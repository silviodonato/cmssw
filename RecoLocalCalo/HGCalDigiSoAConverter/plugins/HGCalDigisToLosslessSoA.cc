#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalLegacyDigiHost.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalLegacyDigiInstances.h"

class HGCalDigisToLosslessSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisToLosslessSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::legacyDigiInstances.size(); ++i) {
      auto const instance = hgcaldigi::legacyDigiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<hgcaldigi::HGCalLegacyDigiHost>(std::string(instance));
      exceptionsTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::sampleExceptionsInstance(instance));
      detIdExceptionsTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::detIdExceptionsInstance(instance));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigis"));
    descriptions.add("hltHgcalDigisSoA", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const&) override {
    constexpr uint32_t sharedMask = 0x1ffff000;
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::legacyDigiInstances.size(); ++instanceIndex) {
      auto const source = event.getHandle(sourceTokens_[instanceIndex]);
      auto const size = source.isValid() ? source->size() : 0;
      hgcaldigi::HGCalLegacyDigiHost output(size);
      auto& view = output.view();
      auto exceptions = std::vector<uint32_t>{};
      auto detIdExceptions = std::vector<uint32_t>{};
      uint32_t previousDetId = 0;

      for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
        auto const& frame = (*source)[i];
        auto const count = frame.data().size();
        if (count > 5) {
          throw cms::Exception("UnsupportedHGCalDigi") << "DetId " << frame.id().rawId() << " has " << count
                                                       << " samples; the lossless SoA supports at most five";
        }

        auto row = view[i];
        auto const detId = frame.id().rawId();
        if (i > 0 && detId >= previousDetId && detId - previousDetId < std::numeric_limits<uint16_t>::max()) {
          row.detIdDelta() = detId - previousDetId;
        } else {
          row.detIdDelta() = std::numeric_limits<uint16_t>::max();
          detIdExceptions.push_back(detId);
        }
        previousDetId = detId;
        row.sampleCount() = count;
        auto const shared = count > 0 ? frame.data()[0].raw() & sharedMask : 0;
        bool common = true;
        uint64_t dataBits = 0;
        uint16_t statusBits = 0;
        auto raw = std::array<uint32_t, 5>{};
        for (std::size_t sample = 0; sample < count; ++sample) {
          raw[sample] = frame.data()[sample].raw();
          common &= (raw[sample] & sharedMask) == shared;
          dataBits |= uint64_t(raw[sample] & 0xfff) << (12 * sample);
          statusBits |= uint16_t((raw[sample] >> 29) & 0x7) << (3 * sample);
        }
        if (common) {
          row.sharedBits() = shared;
          row.statusBits() = statusBits;
          row.dataBits() = dataBits;
          row.exceptionIndex() = std::numeric_limits<uint32_t>::max();
        } else {
          row.sharedBits() = 0;
          row.statusBits() = 0;
          row.dataBits() = 0;
          row.exceptionIndex() = exceptions.size() / 5;
          exceptions.insert(exceptions.end(), raw.begin(), raw.end());
        }
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
      event.emplace(exceptionsTokens_[instanceIndex], std::move(exceptions));
      event.emplace(detIdExceptionsTokens_[instanceIndex], std::move(detIdExceptions));
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<hgcaldigi::HGCalLegacyDigiHost>, 3> outputTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> exceptionsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> detIdExceptionsTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisToLosslessSoA);
