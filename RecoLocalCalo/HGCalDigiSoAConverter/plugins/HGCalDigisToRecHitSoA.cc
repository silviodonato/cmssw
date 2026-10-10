#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiCoding.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

// Preserves exactly the input fields read by HGCalUncalibRecHitRecWeightsAlgo:
// DetId and sample 2's data, mode, threshold, ToA-valid flag and ToA.
// Each instance is written as one range-coded byte stream (HGCalDigiCoding.h); the DetIds are coded as
// the occupancy of the conditions' ordered active-DetId list.
class HGCalDigisToRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisToRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<std::vector<uint8_t>>(std::string(instance));
      geometryTokens_[i] = esConsumes<HGCalGeometry, IdealGeometryRecord>(
          edm::ESInputTag{"", std::string(hgcaldigi::geometryNames[i])});
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigis"));
    descriptions.add("hltHgcalDigisSoA", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override {
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const source = event.getHandle(sourceTokens_[instanceIndex]);
      auto const size = source.isValid() ? source->size() : 0;
      auto const& activeDetIds = eventSetup.getData(geometryTokens_[instanceIndex]).getValidDetIds();
      std::vector<std::size_t> occupied;
      std::vector<hgcaldigi::coding::Sample> samples;
      occupied.reserve(size);
      samples.reserve(size);

      for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
        auto const& frame = (*source)[i];
        if (frame.data().size() < 3) {
          throw cms::Exception("UnsupportedHGCalDigi")
              << "DetId " << frame.id().rawId() << " has fewer than three samples; the rec-hit algorithm reads sample 2";
        }

        auto const detId = frame.id().rawId();
        auto const found = std::lower_bound(activeDetIds.begin(), activeDetIds.end(), frame.id());
        if (found == activeDetIds.end() || *found != frame.id())
          throw cms::Exception("HGCalGeometryIndex")
              << "DetId " << detId << " is absent from geometry " << hgcaldigi::geometryNames[instanceIndex];
        auto const index = static_cast<std::size_t>(std::distance(activeDetIds.begin(), found));
        if (!occupied.empty() && index <= occupied.back())
          throw cms::Exception("HGCalGeometryIndex") << "Input digis are not ordered by geometry index";
        occupied.push_back(index);

        auto const& inTime = frame.data()[2];
        samples.push_back({static_cast<uint16_t>(inTime.data()),
                           inTime.mode(),
                           inTime.threshold(),
                           inTime.getToAValid(),
                           static_cast<uint16_t>(inTime.getToAValid() ? inTime.toa() : 0)});
      }

      event.emplace(
          outputTokens_[instanceIndex],
          hgcaldigi::coding::encode(occupied, samples, activeDetIds, hgcaldigi::cellGroupShifts[instanceIndex]));
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint8_t>>, 3> outputTokens_;
  std::array<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>, 3> geometryTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisToRecHitSoA);
