#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

class HGCalDigisToLosslessSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisToLosslessSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<hgcaldigi::HGCalDigiHost>(std::string(instance));
      detIdDeltasTokens_[i] = produces<std::vector<uint16_t>>(hgcaldigi::sidecarInstance(instance, "DetIdDeltas"));
      sampleCountsTokens_[i] = produces<std::vector<uint8_t>>(hgcaldigi::sidecarInstance(instance, "SampleCounts"));
      sharedBitsTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "SharedBits"));
      statusBitsTokens_[i] = produces<std::vector<uint16_t>>(hgcaldigi::sidecarInstance(instance, "StatusBits"));
      dataBitsTokens_[i] = produces<std::vector<uint64_t>>(hgcaldigi::sidecarInstance(instance, "DataBits"));
      exceptionsTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "Exceptions"));
      detIdExceptionsTokens_[i] =
          produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "DetIdExceptions"));
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
    constexpr uint8_t exceptionFlag = 0x80;
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const source = event.getHandle(sourceTokens_[instanceIndex]);
      auto const size = source.isValid() ? source->size() : 0;
      hgcaldigi::HGCalDigiHost output(size);
      auto& view = output.view();
      std::vector<uint16_t> detIdDeltas;
      std::vector<uint8_t> sampleCounts;
      std::vector<uint32_t> sharedBits;
      std::vector<uint16_t> statusBits;
      std::vector<uint64_t> dataBits;
      std::vector<uint32_t> exceptions;
      std::vector<uint32_t> detIdExceptions;
      detIdDeltas.reserve(size);
      sampleCounts.reserve(size);
      sharedBits.reserve(size);
      statusBits.reserve(size);
      dataBits.reserve(size);
      uint32_t previousDetId = 0;

      for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
        auto const& frame = (*source)[i];
        auto const count = frame.data().size();
        if (count > 5) {
          throw cms::Exception("UnsupportedHGCalDigi") << "DetId " << frame.id().rawId() << " has " << count
                                                       << " samples; the lossless sidecars support at most five";
        }

        auto const detId = frame.id().rawId();
        if (i > 0 && detId >= previousDetId && detId - previousDetId < std::numeric_limits<uint16_t>::max()) {
          detIdDeltas.push_back(detId - previousDetId);
        } else {
          detIdDeltas.push_back(std::numeric_limits<uint16_t>::max());
          detIdExceptions.push_back(detId);
        }
        previousDetId = detId;

        // Keep the standard HGCalDigiSoA fields meaningful as an in-time
        // projection. The sidecars retain the missing raw words and DetIds.
        auto row = view[i];
        row.tctp() = 0;
        row.adcm1() = 0;
        row.adc() = 0;
        row.tot() = 0;
        row.toa() = 0;
        row.cm() = 0;
        row.flags() = hgcal::DIGI_FLAG::Invalid;
        if (count > 2) {
          auto const& inTime = frame.data()[2];
          if (inTime.mode()) {
            row.tctp() = inTime.threshold() ? 2 : 1;
            if (inTime.threshold())
              row.tot() = inTime.data();
          } else {
            row.adc() = inTime.data();
          }
          if (inTime.getToAValid())
            row.toa() = inTime.toa();
          auto const& previous = frame.data()[1];
          if (!previous.mode())
            row.adcm1() = previous.data();
        }

        auto raw = std::array<uint32_t, 5>{};
        auto const shared = count > 0 ? frame.data()[0].raw() & sharedMask : 0;
        bool common = true;
        uint64_t packedData = 0;
        uint16_t packedStatus = 0;
        for (std::size_t sample = 0; sample < count; ++sample) {
          raw[sample] = frame.data()[sample].raw();
          common &= (raw[sample] & sharedMask) == shared;
          packedStatus |= uint16_t((raw[sample] >> 29) & 0x7) << (3 * sample);
          bool const representedInHost =
              (sample == 1 && !frame.data()[sample].mode() && count > 2) ||
              (sample == 2 && (!frame.data()[sample].mode() || frame.data()[sample].threshold()));
          if (!representedInHost)
            packedData |= uint64_t(raw[sample] & 0xfff) << (12 * sample);
        }
        sampleCounts.push_back(uint8_t(count) | (common ? 0 : exceptionFlag));
        sharedBits.push_back(common ? shared : 0);
        statusBits.push_back(common ? packedStatus : 0);
        dataBits.push_back(common ? packedData : 0);
        if (!common)
          exceptions.insert(exceptions.end(), raw.begin(), raw.end());
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
      event.emplace(detIdDeltasTokens_[instanceIndex], std::move(detIdDeltas));
      event.emplace(sampleCountsTokens_[instanceIndex], std::move(sampleCounts));
      event.emplace(sharedBitsTokens_[instanceIndex], std::move(sharedBits));
      event.emplace(statusBitsTokens_[instanceIndex], std::move(statusBits));
      event.emplace(dataBitsTokens_[instanceIndex], std::move(dataBits));
      event.emplace(exceptionsTokens_[instanceIndex], std::move(exceptions));
      event.emplace(detIdExceptionsTokens_[instanceIndex], std::move(detIdExceptions));
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<hgcaldigi::HGCalDigiHost>, 3> outputTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint16_t>>, 3> detIdDeltasTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint8_t>>, 3> sampleCountsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> sharedBitsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint16_t>>, 3> statusBitsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint64_t>>, 3> dataBitsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> exceptionsTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> detIdExceptionsTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisToLosslessSoA);
