#include <array>
#include <cstddef>
#include <cstdint>
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

class HGCalDigisFromRecHitSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigisFromRecHitSoA(edm::ParameterSet const& config) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<hgcaldigi::HGCalDigiHost>(hgcaldigi::withInstance(source, instance));
      detIdsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DetIds")));
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
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const& input = event.get(sourceTokens_[instanceIndex]);
      auto const& detIds = event.get(detIdsTokens_[instanceIndex]);
      auto const& view = input.view();
      auto const size = static_cast<std::size_t>(view.metadata().size());
      if (detIds.size() != size)
        throw cms::Exception("CorruptHGCalDigiSoA") << "DetIds do not match the SoA row count";

      auto output = HGCalDigiCollection{};
      output.reserve(size);
      for (std::size_t i = 0; i < size; ++i) {
        auto const detId = detIds[i];

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
      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<hgcaldigi::HGCalDigiHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> detIdsTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromRecHitSoA);
