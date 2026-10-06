#include <cstdint>
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
#include "FWCore/Utilities/interface/InputTag.h"

class HGCalDataFrameToHGCalDigiSoA : public edm::stream::EDProducer<> {
public:
  explicit HGCalDataFrameToHGCalDigiSoA(edm::ParameterSet const& config)
      : sourceToken_(consumes<HGCalDigiCollection>(config.getParameter<edm::InputTag>("src"))),
        digisToken_(produces<hgcaldigi::HGCalDigiHost>()),
        detIdsToken_(produces<std::vector<uint32_t>>("detIds")),
        sampleIndex_(config.getParameter<unsigned int>("sampleIndex")) {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("simHGCalUnsuppressedDigis", "EE"));
    description.add<unsigned int>("sampleIndex", 2);
    descriptions.add("hgcalDataFrameToHGCalDigiSoA", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const&) override {
    auto const source = event.getHandle(sourceToken_);
    auto const size = source.isValid() ? source->size() : 0;
    hgcaldigi::HGCalDigiHost digis(size);
    auto detIds = std::vector<uint32_t>{};
    detIds.reserve(size);
    auto& view = digis.view();

    for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
      auto const& frame = (*source)[i];
      detIds.push_back(frame.id().rawId());
      auto row = view[i];
      row.tctp() = 0;
      row.adcm1() = 0;
      row.adc() = 0;
      row.tot() = 0;
      row.toa() = 0;
      row.cm() = 0;
      // Legacy ADC/ToT scales and channel indexing differ from the raw-data SoA.
      row.flags() = hgcal::DIGI_FLAG::Invalid;

      if (sampleIndex_ >= static_cast<unsigned int>(frame.size()))
        continue;
      auto const& sample = frame[sampleIndex_];
      if (sample.mode()) {
        // A mode-1 sample with threshold set is a complete ToT measurement;
        // without threshold it is a busy/in-progress measurement.
        row.tctp() = sample.threshold() ? 2 : 1;
        if (sample.threshold())
          row.tot() = sample.data();
      } else {
        row.adc() = sample.data();
      }
      if (sample.getToAValid())
        row.toa() = sample.toa();
      if (sampleIndex_ > 0) {
        auto const& previous = frame[sampleIndex_ - 1];
        if (!previous.mode())
          row.adcm1() = previous.data();
      }
    }

    event.emplace(digisToken_, std::move(digis));
    event.emplace(detIdsToken_, std::move(detIds));
  }

  edm::EDGetTokenT<HGCalDigiCollection> const sourceToken_;
  edm::EDPutTokenT<hgcaldigi::HGCalDigiHost> const digisToken_;
  edm::EDPutTokenT<std::vector<uint32_t>> const detIdsToken_;
  unsigned int const sampleIndex_;
};

DEFINE_FWK_MODULE(HGCalDataFrameToHGCalDigiSoA);
