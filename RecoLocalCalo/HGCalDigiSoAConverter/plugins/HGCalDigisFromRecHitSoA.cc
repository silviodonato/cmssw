#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiCoding.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

// Decodes the range-coded streams of HGCalDigisToRecHitSoA into five-sample legacy digis,
// with the original recHit-relevant fields in sample 2.
class HGCalDigisFromRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisFromRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<std::vector<uint8_t>>(hgcaldigi::withInstance(source, instance));
      geometryTokens_[i] = esConsumes<HGCalGeometry, IdealGeometryRecord>(
          edm::ESInputTag{"", std::string(hgcaldigi::geometryNames[i])});
      outputTokens_[i] = produces<HGCalDigiCollection>(std::string(instance));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigisSoA"));
    descriptions.add("hltHgcalDigisDecompressed", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override {
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const& stream = event.get(sourceTokens_[instanceIndex]);
      auto const& activeDetIds = eventSetup.getData(geometryTokens_[instanceIndex]).getValidDetIds();
      std::vector<std::size_t> occupied;
      std::vector<hgcaldigi::coding::Sample> samples;
      hgcaldigi::coding::decode(stream, activeDetIds, hgcaldigi::cellGroupShifts[instanceIndex], occupied, samples);

      auto output = HGCalDigiCollection{};
      output.reserve(occupied.size());
      for (std::size_t i = 0; i < occupied.size(); ++i) {
        auto const& sample = samples[i];
        HGCSample inTime;
        inTime.setMode(sample.mode);
        inTime.setThreshold(sample.threshold);
        inTime.setToAValid(sample.toaValid);
        inTime.setData(sample.data);
        if (sample.toaValid)
          inTime.setToA(sample.toa);
        auto frame = HGCalDataFrame(activeDetIds[occupied[i]]);
        frame.resize(5);
        frame.setSample(2, inTime);
        output.push_back(frame);
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<std::vector<uint8_t>>, 3> sourceTokens_;
  std::array<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>, 3> geometryTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromRecHitSoA);
