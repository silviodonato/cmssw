#ifndef DATAFORMATS_HGCRECHIT_HGCUNCALIBRATEDRECHITCOMPRESSEDS_SORTED_H
#define DATAFORMATS_HGCRECHIT_HGCUNCALIBRATEDRECHITCOMPRESSEDS_SORTED_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "DataFormats/Common/interface/CMS_CLASS_VERSION.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"

// The deliberately plural name preserves the friendly name of the previous
// edm::SortedCollection<HGCUncalibratedRecHitCompressed> product.
class HGCUncalibratedRecHitCompressedsSorted {
public:
  using value_type = HGCUncalibratedRecHitCompressed;
  using key_type = DetId;
  using size_type = std::vector<value_type>::size_type;

  enum class Encoding : uint8_t { kGeometryIndexDelta = 1 };
  using const_iterator = std::vector<value_type>::const_iterator;

  HGCUncalibratedRecHitCompressedsSorted() = default;

  void reserve(size_type size) { hits_.reserve(size); }
  bool empty() const { return hits_.empty(); }
  size_type size() const { return hits_.size(); }

  // hit.id() holds the index_type difference from the previous index in the
  // corresponding HGCalGeometry valid-DetId list.
  void push_back(const value_type& hit) { hits_.push_back(hit); }

  void setEncoding(Encoding encoding) { encoding_ = encoding; }
  bool isGeometryIndexEncoded() const { return encoding_ == Encoding::kGeometryIndexDelta; }

  const_iterator begin() const { return hits_.begin(); }
  const_iterator cbegin() const { return begin(); }
  const_iterator end() const { return hits_.end(); }
  const_iterator cend() const { return end(); }

  // Reconstruct the full geometry index without materializing it in the
  // persisted hit. The callback receives (compressed hit, full index).
  template <typename F>
  void forEachGeometryIndex(F&& function) const {
    uint32_t geometryIndex = 0;
    for (const auto& hit : hits_) {
      geometryIndex += hit.id().rawId();
      function(hit, geometryIndex);
    }
  }

  // The index deltas are already computed by the compressor.
  void post_insert();

  CMS_CLASS_VERSION(3)

private:
  std::vector<value_type> hits_;
  Encoding encoding_ = Encoding::kGeometryIndexDelta;
};

#endif
