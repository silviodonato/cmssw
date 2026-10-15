#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalDenseIndexMap_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalDenseIndexMap_h

#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include "CondFormats/HGCalObjects/interface/HGCalMappingParameterHost.h"
#include "FWCore/Utilities/interface/Exception.h"

namespace hgcaldigi {

  // Two-way map between HGCal DetIds and the electronics dense index of HGCalDenseIndexInfoSoA.
  //
  // The available module map (modulelocator_CEminus_V15p5.txt) describes the z-minus endcap only, so
  // z-plus DetIds are folded onto z-minus by flipping the zside bit (bit 25 for both HGCSiliconDetId and
  // HGCScintillatorDetId) and stored with an offset of size():
  //   [0, size())          z-minus DetId, dense index as is
  //   [size(), 2 size())   z-plus DetId, dense index of its z-minus mirror
  // Values from escapeBase() upwards are reserved for DetIds absent from the map.
  class HGCalDenseIndexMap {
  public:
    static constexpr uint32_t zsideBit = 1u << 25;

    explicit HGCalDenseIndexMap(hgcal::HGCalDenseIndexInfoHost const& info) {
      auto const view = info.const_view();
      size_ = static_cast<uint32_t>(view.metadata().size());
      detIds_.resize(size_);
      byDetId_.reserve(size_);
      for (uint32_t i = 0; i < size_; ++i) {
        auto const detId = view[i].detid();
        detIds_[i] = detId;
        // Only z-minus cells are mapped; anything else could not be folded unambiguously.
        if (detId != 0 && (detId & zsideBit))
          byDetId_.emplace_back(detId, i);
      }
      // A DetId shared by several dense indices (e.g. calibration cells) maps to the first one,
      // which decodes back to the same DetId, so the round trip stays exact.
      std::stable_sort(
          byDetId_.begin(), byDetId_.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
      byDetId_.erase(
          std::unique(
              byDetId_.begin(), byDetId_.end(), [](auto const& a, auto const& b) { return a.first == b.first; }),
          byDetId_.end());
    }

    uint32_t size() const { return size_; }
    uint32_t escapeBase() const { return 2 * size_; }
    std::size_t mappedDetIds() const { return byDetId_.size(); }

    std::optional<uint32_t> index(uint32_t detId) const {
      bool const zplus = !(detId & zsideBit);
      auto const key = detId | zsideBit;
      auto const it = std::lower_bound(
          byDetId_.begin(), byDetId_.end(), key, [](auto const& entry, uint32_t value) { return entry.first < value; });
      if (it == byDetId_.end() || it->first != key)
        return std::nullopt;
      return zplus ? it->second + size_ : it->second;
    }

    uint32_t detId(uint32_t index) const {
      if (index >= escapeBase())
        throw cms::Exception("CorruptHGCalDigiSoA") << "dense index " << index << " is outside the map";
      return index < size_ ? detIds_[index] : (detIds_[index - size_] & ~zsideBit);
    }

  private:
    uint32_t size_ = 0;
    std::vector<uint32_t> detIds_;
    std::vector<std::pair<uint32_t, uint32_t>> byDetId_;
  };

}  // namespace hgcaldigi

#endif
