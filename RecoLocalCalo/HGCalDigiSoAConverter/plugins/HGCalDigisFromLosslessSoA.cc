#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

class HGCalDigisFromLosslessSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisFromLosslessSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<hgcaldigi::HGCalDigiHost>(hgcaldigi::withInstance(source, instance));
      detIdDeltasTokens_[i] = consumes<std::vector<uint16_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DetIdDeltas")));
      sampleCountsTokens_[i] = consumes<std::vector<uint8_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "SampleCounts")));
      sharedBitsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "SharedBits")));
      statusBitsTokens_[i] = consumes<std::vector<uint16_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "StatusBits")));
      dataBitsTokens_[i] = consumes<std::vector<uint64_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DataBits")));
      exceptionsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "Exceptions")));
      detIdExceptionsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DetIdExceptions")));
      outputTokens_[i] = produces<HGCalDigiCollection>(std::string(instance));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigisSoA"));
    descriptions.add("hltHgcalDigisDecompressed", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const&) override {
    constexpr uint8_t exceptionFlag = 0x80;
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const& input = event.get(sourceTokens_[instanceIndex]);
      auto const& detIdDeltas = event.get(detIdDeltasTokens_[instanceIndex]);
      auto const& sampleCounts = event.get(sampleCountsTokens_[instanceIndex]);
      auto const& sharedBits = event.get(sharedBitsTokens_[instanceIndex]);
      auto const& statusBits = event.get(statusBitsTokens_[instanceIndex]);
      auto const& dataBits = event.get(dataBitsTokens_[instanceIndex]);
      auto const& exceptions = event.get(exceptionsTokens_[instanceIndex]);
      auto const& detIdExceptions = event.get(detIdExceptionsTokens_[instanceIndex]);
      auto const& view = input.view();
      auto const size = static_cast<std::size_t>(view.metadata().size());
      if (detIdDeltas.size() != size || sampleCounts.size() != size || sharedBits.size() != size ||
          statusBits.size() != size || dataBits.size() != size || exceptions.size() % 5 != 0) {
        throw cms::Exception("CorruptHGCalDigiSoA") << "The lossless sidecars do not match the HGCalDigiSoA row count";
      }

      auto output = HGCalDigiCollection{};
      output.reserve(size);
      uint32_t previousDetId = 0;
      std::size_t nextDetIdException = 0;
      std::size_t nextSampleException = 0;

      for (std::size_t i = 0; i < size; ++i) {
        uint32_t detId;
        if (detIdDeltas[i] == std::numeric_limits<uint16_t>::max()) {
          if (nextDetIdException >= detIdExceptions.size())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " is missing its DetId exception";
          detId = detIdExceptions[nextDetIdException++];
        } else {
          if (i == 0 || previousDetId > std::numeric_limits<uint32_t>::max() - detIdDeltas[i])
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has an invalid DetId delta";
          detId = previousDetId + detIdDeltas[i];
        }
        previousDetId = detId;

        unsigned int const count = sampleCounts[i] & ~exceptionFlag;
        if (count > 5)
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has sampleCount=" << unsigned(count);
        auto frame = HGCalDataFrame(DetId(detId));
        frame.resize(count);
        auto raw = std::array<uint32_t, 5>{};
        if (sampleCounts[i] & exceptionFlag) {
          if (nextSampleException >= exceptions.size() / 5)
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " is missing sample exceptions";
          for (unsigned int sample = 0; sample < count; ++sample)
            raw[sample] = exceptions[5 * nextSampleException + sample];
          ++nextSampleException;
        } else {
          auto const row = view[i];
          for (unsigned int sample = 0; sample < count; ++sample) {
            auto const status = (statusBits[i] >> (3 * sample)) & 0x7;
            auto const mode = (status & 0x2) != 0;
            auto const threshold = (status & 0x4) != 0;
            uint32_t data = (dataBits[i] >> (12 * sample)) & 0xfff;
            if (sample == 1 && !mode && count > 2)
              data = row.adcm1();
            else if (sample == 2 && !mode)
              data = row.adc();
            else if (sample == 2 && threshold)
              data = row.tot();
            if (data > 0xfff)
              throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has out-of-range sample data";
            raw[sample] = sharedBits[i] | (status << 29) | data;
          }
        }
        for (unsigned int sample = 0; sample < count; ++sample)
          frame.setSample(sample, HGCSample(raw[sample]));
        output.push_back(frame);
      }
      if (nextDetIdException != detIdExceptions.size() || nextSampleException != exceptions.size() / 5)
        throw cms::Exception("CorruptHGCalDigiSoA") << "Unused lossless exceptions remain after decoding";

      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<hgcaldigi::HGCalDigiHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint16_t>>, 3> detIdDeltasTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint8_t>>, 3> sampleCountsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> sharedBitsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint16_t>>, 3> statusBitsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint64_t>>, 3> dataBitsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> exceptionsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> detIdExceptionsTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromLosslessSoA);
