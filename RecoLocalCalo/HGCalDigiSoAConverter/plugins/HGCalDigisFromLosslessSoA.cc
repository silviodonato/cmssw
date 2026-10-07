#include <array>
#include <cstdint>
#include <limits>
#include <string>
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

class HGCalDigisFromLosslessSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisFromLosslessSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::legacyDigiInstances.size(); ++i) {
      auto const instance = hgcaldigi::legacyDigiInstances[i];
      sourceTokens_[i] = consumes<hgcaldigi::HGCalLegacyDigiHost>(hgcaldigi::withInstance(source, instance));
      exceptionsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sampleExceptionsInstance(instance)));
      detIdExceptionsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::detIdExceptionsInstance(instance)));
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
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::legacyDigiInstances.size(); ++instanceIndex) {
      auto const& input = event.get(sourceTokens_[instanceIndex]);
      auto const& exceptions = event.get(exceptionsTokens_[instanceIndex]);
      auto const& detIdExceptions = event.get(detIdExceptionsTokens_[instanceIndex]);
      if (exceptions.size() % 5 != 0)
        throw cms::Exception("CorruptHGCalDigiSoA") << "The exceptions product contains an incomplete frame";
      auto const& view = input.view();
      auto output = HGCalDigiCollection{};
      output.reserve(view.metadata().size());
      uint32_t previousDetId = 0;
      std::size_t nextDetIdException = 0;

      for (int32_t i = 0; i < view.metadata().size(); ++i) {
        auto const row = view[i];
        uint32_t detId;
        if (row.detIdDelta() == std::numeric_limits<uint16_t>::max()) {
          if (nextDetIdException >= detIdExceptions.size())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " is missing its DetId exception";
          detId = detIdExceptions[nextDetIdException++];
        } else {
          if (i == 0 || previousDetId > std::numeric_limits<uint32_t>::max() - row.detIdDelta())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has an invalid DetId delta";
          detId = previousDetId + row.detIdDelta();
        }
        previousDetId = detId;
        auto const count = row.sampleCount();
        if (count > 5) {
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has sampleCount=" << unsigned(count);
        }
        auto frame = HGCalDataFrame(DetId(detId));
        frame.resize(count);
        auto raw = std::array<uint32_t, 5>{};
        auto const exceptionIndex = row.exceptionIndex();
        if (exceptionIndex != std::numeric_limits<uint32_t>::max()) {
          if (exceptionIndex >= exceptions.size() / 5)
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has invalid exceptionIndex";
          for (unsigned int sample = 0; sample < count; ++sample)
            raw[sample] = exceptions[5 * exceptionIndex + sample];
        } else {
          for (unsigned int sample = 0; sample < count; ++sample) {
            raw[sample] = row.sharedBits() | (((row.statusBits() >> (3 * sample)) & 0x7) << 29) |
                          ((row.dataBits() >> (12 * sample)) & 0xfff);
          }
        }
        for (unsigned int sample = 0; sample < count; ++sample)
          frame.setSample(sample, HGCSample(raw[sample]));
        output.push_back(frame);
      }
      if (nextDetIdException != detIdExceptions.size())
        throw cms::Exception("CorruptHGCalDigiSoA") << "Unused DetId exceptions remain after decoding";

      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<hgcaldigi::HGCalLegacyDigiHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> exceptionsTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> detIdExceptionsTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromLosslessSoA);
