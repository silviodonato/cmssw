#include <array>
#include <cstddef>
#include <cstring>
#include <string_view>

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace {
  constexpr std::array<std::string_view, 3> hitInstances = {
      "HGCEEUncalibRecHits", "HGCHEFUncalibRecHits", "HGCHEBUncalibRecHits"};

  bool sameBits(float a, float b) { return std::memcmp(&a, &b, sizeof(float)) == 0; }

  void compareHit(HGCUncalibratedRecHit const& a,
                  HGCUncalibratedRecHit const& b,
                  edm::EventID const& eventId,
                  std::string_view instance,
                  std::size_t index) {
    auto const fail = [&](char const* field) {
      throw cms::Exception("HGCalUncalibRecHitMismatch")
          << "Event " << eventId << ", " << instance << ", hit " << index << ", DetId " << a.id().rawId()
          << ": " << field << " differs";
    };
    if (a.id().rawId() != b.id().rawId()) fail("DetId");
    if (!sameBits(a.amplitude(), b.amplitude())) fail("amplitude");
    if (!sameBits(a.pedestal(), b.pedestal())) fail("pedestal");
    if (!sameBits(a.jitter(), b.jitter())) fail("jitter");
    if (!sameBits(a.chi2(), b.chi2())) fail("chi2");
    if (!sameBits(a.outOfTimeEnergy(), b.outOfTimeEnergy())) fail("outOfTimeEnergy");
    if (!sameBits(a.outOfTimeChi2(), b.outOfTimeChi2())) fail("outOfTimeChi2");
    if (!sameBits(a.jitterError(), b.jitterError())) fail("jitterError");
    if (a.jitterErrorBits() != b.jitterErrorBits()) fail("jitterErrorBits");
    if (a.flags() != b.flags()) fail("flags");
  }
}  // namespace

class HGCalUncalibRecHitRoundTripValidator : public edm::one::EDAnalyzer<> {
public:
  explicit HGCalUncalibRecHitRoundTripValidator(edm::ParameterSet const& config) {
    auto const original = config.getParameter<edm::InputTag>("original");
    auto const restored = config.getParameter<edm::InputTag>("restored");
    for (std::size_t i = 0; i < hitInstances.size(); ++i) {
      auto const instance = std::string(hitInstances[i]);
      originalTokens_[i] = consumes<HGCUncalibratedRecHitCollection>(
          edm::InputTag(original.label(), instance, original.process()));
      restoredTokens_[i] = consumes<HGCUncalibratedRecHitCollection>(
          edm::InputTag(restored.label(), instance, restored.process()));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("original", edm::InputTag("hltHGCalUncalibRecHit"));
    description.add<edm::InputTag>("restored", edm::InputTag("hltHGCalUncalibRecHitDecompressed"));
    descriptions.add("hgcalUncalibRecHitRoundTripValidator", description);
  }

private:
  void analyze(edm::Event const& event, edm::EventSetup const&) override {
    ++events_;
    for (std::size_t instance = 0; instance < hitInstances.size(); ++instance) {
      auto const& original = event.get(originalTokens_[instance]);
      auto const& restored = event.get(restoredTokens_[instance]);
      if (original.size() != restored.size()) {
        throw cms::Exception("HGCalUncalibRecHitMismatch")
            << "Event " << event.id() << ", " << hitInstances[instance] << ": original has " << original.size()
            << " hits, restored has " << restored.size();
      }
      hits_[instance] += original.size();
      for (std::size_t i = 0; i < original.size(); ++i)
        compareHit(original[i], restored[i], event.id(), hitInstances[instance], i);
    }
  }

  void endJob() override {
    edm::LogVerbatim("HGCalUncalibRecHitRoundTripValidator")
        << "Compared " << events_ << " events with identical uncalibrated recHits: EE " << hits_[0]
        << ", HEfront " << hits_[1] << ", HEback " << hits_[2];
  }

  std::array<edm::EDGetTokenT<HGCUncalibratedRecHitCollection>, 3> originalTokens_;
  std::array<edm::EDGetTokenT<HGCUncalibratedRecHitCollection>, 3> restoredTokens_;
  std::array<std::size_t, 3> hits_{};
  std::size_t events_ = 0;
};

DEFINE_FWK_MODULE(HGCalUncalibRecHitRoundTripValidator);
