#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
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
      detIdDeltasTokens_[i] = consumes<std::vector<uint16_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DetIdDeltas")));
      detIdExceptionsTokens_[i] = consumes<std::vector<uint32_t>>(
          hgcaldigi::withInstance(source, hgcaldigi::sidecarInstance(instance, "DetIdExceptions")));
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
      auto const& detIdDeltas = event.get(detIdDeltasTokens_[instanceIndex]);
      auto const& detIdExceptions = event.get(detIdExceptionsTokens_[instanceIndex]);
      auto const& view = input.view();
      auto const size = static_cast<std::size_t>(view.metadata().size());
      if (detIdDeltas.size() != size)
        throw cms::Exception("CorruptHGCalDigiSoA") << "DetId deltas do not match the SoA row count";

      auto output = HGCalDigiCollection{};
      output.reserve(size);
      uint32_t previousDetId = 0;
      std::size_t nextDetIdException = 0;

      for (std::size_t i = 0; i < size; ++i) {
        uint32_t detId;
        if (detIdDeltas[i] == std::numeric_limits<uint16_t>::max()) {
          if (nextDetIdException >= detIdExceptions.size())
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " is missing its DetId exception";
          detId = detIdExceptions[nextDetIdException++];
        } else {
          if (i == 0 || previousDetId > std::numeric_limits<uint32_t>::max() - detIdDeltas[i])
            throw cms::Exception("CorruptHGCalDigiSoA") << "SoA row " << i << " has an invalid DetId delta";
          detId = previousDetId + detIdDeltas[i];
        }
        previousDetId = detId;

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
      if (nextDetIdException != detIdExceptions.size())
        throw cms::Exception("CorruptHGCalDigiSoA") << "Unused DetId exceptions remain after decoding";

      event.emplace(outputTokens_[instanceIndex], std::move(output));
    }
  }

  std::array<edm::EDGetTokenT<hgcaldigi::HGCalDigiHost>, 3> sourceTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint16_t>>, 3> detIdDeltasTokens_;
  std::array<edm::EDGetTokenT<std::vector<uint32_t>>, 3> detIdExceptionsTokens_;
  std::array<edm::EDPutTokenT<HGCalDigiCollection>, 3> outputTokens_;
};

DEFINE_FWK_MODULE(HGCalDigisFromRecHitSoA);
