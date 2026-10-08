#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCompressedHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
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
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

// Preserves exactly the input fields read by HGCalUncalibRecHitRecWeightsAlgo:
// DetId and sample 2's data, mode, threshold, ToA-valid flag and ToA.
// The SoA has no DetId column. A compact geometry-index delta stream
// (one byte per row, plus a 32-bit overflow vector for deltas of 255 or more) identifies each row using the conditions' ordered active-DetId list.
class HGCalDigisToRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisToRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<hgcaldigi::HGCalDigiCompressedHost>(std::string(instance));
      indexDeltasTokens_[i] = produces<std::vector<uint8_t>>(hgcaldigi::sidecarInstance(instance, "IndexDeltas"));
      indexDeltaOverflowsTokens_[i] =
          produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "IndexDeltaOverflows"));
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
      hgcaldigi::HGCalDigiCompressedHost output(size);
      auto& view = output.view();
      auto const& activeDetIds = eventSetup.getData(geometryTokens_[instanceIndex]).getValidDetIds();
      std::vector<uint8_t> indexDeltas;
      indexDeltas.reserve(size);
      std::vector<uint32_t> indexDeltaOverflows;
      std::size_t previousIndex = 0;

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
        if (index < previousIndex)
          throw cms::Exception("HGCalGeometryIndex") << "Input digis are not ordered by geometry index";
        auto delta = index - previousIndex;
        if (delta >= 255) {
          // 255 is an escape: the remainder (delta - 255) is stored in the overflow vector.
          indexDeltas.push_back(255);
          indexDeltaOverflows.push_back(static_cast<uint32_t>(delta - 255));
        } else {
          indexDeltas.push_back(static_cast<uint8_t>(delta));
        }
        previousIndex = index;

        auto const& inTime = frame.data()[2];
        auto row = view[i];
        // Private legacy-digi encoding: tctp bits 0 and 1 carry mode and threshold; ToA-valid is implied by a stored ToA.
        // The 12-bit in-time data is stored as ToT in mode 1 and as ADC otherwise.
        row.setTctp((inTime.mode() ? 1 : 0) | (inTime.threshold() ? 2 : 0));
        if (inTime.mode())
          row.setTot(inTime.data());
        else
          row.setAdc(inTime.data());
        if (inTime.getToAValid())
          row.setToa(inTime.toa());
        else
          row.clearToa();
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
      event.emplace(indexDeltasTokens_[instanceIndex], std::move(indexDeltas));
      event.emplace(indexDeltaOverflowsTokens_[instanceIndex], std::move(indexDeltaOverflows));
    }
  }

  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<hgcaldigi::HGCalDigiCompressedHost>, 3> outputTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint8_t>>, 3> indexDeltasTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> indexDeltaOverflowsTokens_;
  std::array<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>, 3> geometryTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisToRecHitSoA);
