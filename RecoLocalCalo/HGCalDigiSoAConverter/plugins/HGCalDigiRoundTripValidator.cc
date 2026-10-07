#include <array>
#include <cstddef>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalLegacyDigiInstances.h"

class HGCalDigiRoundTripValidator : public edm::stream::EDAnalyzer<> {
public:
  explicit HGCalDigiRoundTripValidator(edm::ParameterSet const& config) {
    auto const original = config.getParameter<edm::InputTag>("original");
    auto const restored = config.getParameter<edm::InputTag>("restored");
    for (std::size_t i = 0; i < hgcaldigi::legacyDigiInstances.size(); ++i) {
      auto const instance = hgcaldigi::legacyDigiInstances[i];
      originalTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(original, instance));
      restoredTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(restored, instance));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("original", edm::InputTag("hltHgcalDigis"));
    description.add<edm::InputTag>("restored", edm::InputTag("hltHgcalDigisDecompressed"));
    descriptions.add("hgcalDigiRoundTripValidator", description);
  }

private:
  void analyze(edm::Event const& event, edm::EventSetup const&) override {
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::legacyDigiInstances.size(); ++instanceIndex) {
      auto const& original = event.get(originalTokens_[instanceIndex]);
      auto const& restored = event.get(restoredTokens_[instanceIndex]);
      if (original.size() != restored.size()) {
        throw cms::Exception("HGCalDigiRoundTripMismatch")
            << "Event " << event.id() << ": original has " << original.size() << " frames, restored has "
            << restored.size();
      }

      for (std::size_t i = 0; i < original.size(); ++i) {
        auto const& before = original[i];
        auto const& after = restored[i];
        if (before.id().rawId() != after.id().rawId() || before.data().size() != after.data().size()) {
          throw cms::Exception("HGCalDigiRoundTripMismatch")
              << "Event " << event.id() << ", frame " << i << ": DetId or sample count differs";
        }
        for (std::size_t sample = 0; sample < before.data().size(); ++sample) {
          if (before.data()[sample].raw() != after.data()[sample].raw()) {
            throw cms::Exception("HGCalDigiRoundTripMismatch")
                << "Event " << event.id() << ", frame " << i << ", sample " << sample << ": original word 0x"
                << std::hex << before.data()[sample].raw() << ", restored word 0x" << after.data()[sample].raw();
          }
        }
      }
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> originalTokens_;
  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> restoredTokens_;
};

DEFINE_FWK_MODULE(HGCalDigiRoundTripValidator);
