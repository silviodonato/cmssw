#include <array>
#include <cstddef>
#include <cstdint>
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

// Preserves exactly the input fields read by HGCalUncalibRecHitRecWeightsAlgo:
// DetId and sample 2's data, mode, threshold, ToA-valid flag and ToA.
// The standard SoA has no DetId column, so a row-aligned sidecar stores raw DetIds.
class HGCalDigisToRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisToRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<hgcaldigi::HGCalDigiHost>(std::string(instance));
      detIdsTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "DetIds"));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigis"));
    descriptions.add("hltHgcalDigisSoA", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const&) override {
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const source = event.getHandle(sourceTokens_[instanceIndex]);
      auto const size = source.isValid() ? source->size() : 0;
      hgcaldigi::HGCalDigiHost output(size);
      auto& view = output.view();
      std::vector<uint32_t> detIds;
      detIds.reserve(size);

      for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
        auto const& frame = (*source)[i];
        if (frame.data().size() < 3) {
          throw cms::Exception("UnsupportedHGCalDigi")
              << "DetId " << frame.id().rawId() << " has fewer than three samples; the rec-hit algorithm reads sample 2";
        }

        detIds.push_back(frame.id().rawId());

        auto const& inTime = frame.data()[2];
        auto row = view[i];
        // Private legacy-digi encoding within the standard SoA type:
        // tctp bits 0, 1, 2 carry mode, threshold, and ToA-valid respectively.
        // Rows are marked Invalid because these are not native ECON-D digis.
        row.tctp() = (inTime.mode() ? 1 : 0) | (inTime.threshold() ? 2 : 0) |
                     (inTime.getToAValid() ? 4 : 0);
        row.adcm1() = 0;
        row.adc() = inTime.mode() ? 0 : inTime.data();
        row.tot() = inTime.mode() ? inTime.data() : 0;
        row.toa() = inTime.getToAValid() ? inTime.toa() : 0;
        row.cm() = 0;
        row.flags() = hgcal::DIGI_FLAG::Invalid;
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
      event.emplace(detIdsTokens_[instanceIndex], std::move(detIds));
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<hgcaldigi::HGCalDigiHost>, 3> outputTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> detIdsTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisToRecHitSoA);
