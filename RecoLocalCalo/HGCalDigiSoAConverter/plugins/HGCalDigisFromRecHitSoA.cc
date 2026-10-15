#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "CondFormats/DataRecord/interface/HGCalDenseIndexInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingParameterHost.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDenseIndexMap.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

class HGCalDigisFromRecHitSoA : public edm::global::EDProducer<edm::RunCache<hgcaldigi::HGCalDenseIndexMap>> {
public:
  explicit HGCalDigisFromRecHitSoA(edm::ParameterSet const& config)
      : denseIndexInfoToken_(esConsumes<edm::Transition::BeginRun>()) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<hgcaldigi::HGCalDigiHost>(hgcaldigi::withInstance(source, instance));
      denseIndicesTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DenseIndices")));
      unmappedTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "UnmappedDetIds")));
      outputTokens_[i] = produces<HGCalDigiCollection>(std::string(instance));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigisSoA"));
    descriptions.add("hltHgcalDigisDecompressed", description);
  }

  std::shared_ptr<hgcaldigi::HGCalDenseIndexMap> globalBeginRun(edm::Run const&,
                                                                edm::EventSetup const& setup) const override {
    return std::make_shared<hgcaldigi::HGCalDenseIndexMap>(setup.getData(denseIndexInfoToken_));
  }

  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

private:
  void produce(edm::StreamID, edm::Event& event, edm::EventSetup const&) const override {
    auto const& map = *runCache(event.getRun().index());
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const& input = event.get(sourceTokens_[instanceIndex]);
      auto const& denseIndices = event.get(denseIndicesTokens_[instanceIndex]);
      auto const& unmapped = event.get(unmappedTokens_[instanceIndex]);
      auto const& view = input.view();
      auto const size = static_cast<std::size_t>(view.metadata().size());
      if (denseIndices.size() != size)
        throw cms::Exception("CorruptHGCalDigiSoA") << "dense indices do not match the SoA row count";

      auto output = HGCalDigiCollection{};
      output.reserve(size);
      for (std::size_t i = 0; i < size; ++i) {
        auto const index = denseIndices[i];
        uint32_t detId;
        if (index < map.escapeBase()) {
          detId = map.detId(index);
        } else {
          auto const position = index - map.escapeBase();
          if (position >= unmapped.size())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " refers to a missing unmapped DetId";
          detId = unmapped[position];
        }

        auto const row = view[i];
        auto const status = row.tctp();
        if (status & ~uint8_t(0x7))
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has invalid in-time status";
        bool const mode = status & 0x1;
        bool const threshold = status & 0x2;
        bool const toaValid = status & 0x4;
        auto const data = mode ? row.tot() : row.adc();
        if (data > 0xfff || (toaValid && row.toa() > 0x3ff))
          throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has out-of-range sample data";

        HGCSample inTime;
        inTime.setMode(mode);
        inTime.setThreshold(threshold);
        inTime.setToAValid(toaValid);
        inTime.setData(data);
        if (toaValid)
          inTime.setToA(row.toa());
        auto frame = HGCalDataFrame(DetId(detId));
        frame.resize(5);
        frame.setSample(2, inTime);
        output.push_back(frame);
      }
      // Rows are stored in dense-index order; restore the DetId order of the original collection.
      output.sort();
      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  edm::ESGetToken<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd> denseIndexInfoToken_;
  std::array<edm::EDGetTokenT<hgcaldigi::HGCalDigiHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> denseIndicesTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> unmappedTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromRecHitSoA);
