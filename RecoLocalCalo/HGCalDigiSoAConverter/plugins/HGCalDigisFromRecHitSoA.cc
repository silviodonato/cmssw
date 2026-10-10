#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCompressedHost.h"
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
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalToaPacking.h"

class HGCalDigisFromRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisFromRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<hgcaldigi::HGCalDigiCompressedHost>(hgcaldigi::withInstance(source, instance));
      indexDeltasTokens_[i] = consumes<std::vector<uint8_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "IndexDeltas")));
      toaTokens_[i] =
          consumes<std::vector<uint8_t>>(hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "Toa")));
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
      auto const& input = event.get(sourceTokens_[instanceIndex]);
      auto const& indexDeltas = event.get(indexDeltasTokens_[instanceIndex]);
      auto const& toas = event.get(toaTokens_[instanceIndex]);
      auto const& activeDetIds = eventSetup.getData(geometryTokens_[instanceIndex]).getValidDetIds();
      auto const& view = input.view();
      auto const size = static_cast<std::size_t>(view.metadata().size());

      auto output = HGCalDigiCollection{};
      output.reserve(size);
      std::size_t geometryIndex = 0;
      std::size_t nextDelta = 0;
      std::size_t toaCount = 0;
      for (std::size_t i = 0; i < size; ++i)
        toaCount += view[i].toaValid();
      if (toas.size() != hgcaldigi::toa::packedSize(toaCount))
        throw cms::Exception("CorruptHGCalDigiSoA") << "ToA stream size does not match the number of valid ToAs";
      std::size_t nextToa = 0;

      for (std::size_t i = 0; i < size; ++i) {
        uint8_t delta;
        do {
          if (nextDelta >= indexDeltas.size())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " is missing its geometry index delta";
          delta = indexDeltas[nextDelta++];
          if (geometryIndex >= activeDetIds.size() || delta > activeDetIds.size() - 1 - geometryIndex)
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has an out-of-range geometry index";
          geometryIndex += delta;
        } while (delta == 255);
        auto const detId = activeDetIds[geometryIndex];

        auto const row = view[i];
        auto const status = row.tctp();
        if (status & ~uint8_t(0x7))
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has invalid in-time status";
        bool const mode = status & 0x1;
        bool const threshold = status & 0x2;
        bool const toaValid = status & 0x4;
        auto const data = mode ? row.tot() : row.adc();
        if (data > 0xfff)
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has out-of-range sample data";

        HGCSample inTime;
        inTime.setMode(mode);
        inTime.setThreshold(threshold);
        inTime.setToAValid(toaValid);
        inTime.setData(data);
        if (toaValid)
          inTime.setToA(hgcaldigi::toa::unpack(toas, toaCount, nextToa++));
        auto frame = HGCalDataFrame(detId);
        frame.resize(5);
        frame.setSample(2, inTime);
        output.push_back(frame);
      }
      if (nextDelta != indexDeltas.size())
        throw cms::Exception("CorruptHGCalDigiSoA") << "Unused geometry index deltas remain after decoding";

      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<hgcaldigi::HGCalDigiCompressedHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint8_t>>, 3> indexDeltasTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint8_t>>, 3> toaTokens_;
  std::array<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>, 3> geometryTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromRecHitSoA);
