#include <cstdint>
#include <utility>
#include <vector>

#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

class QIE10ToHGCalDigiSoA : public edm::stream::EDProducer<> {
public:
  explicit QIE10ToHGCalDigiSoA(edm::ParameterSet const& config)
      : sourceToken_(consumes<QIE10DigiCollection>(config.getParameter<edm::InputTag>("src"))),
        digisToken_(produces<hgcaldigi::HGCalDigiHost>()),
        detIdsToken_(produces<std::vector<uint32_t>>("detIds")) {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHcalDigis"));
    descriptions.add("qie10ToHGCalDigiSoA", description);
  }

private:
  void produce(edm::Event& event, edm::EventSetup const&) override {
    auto const source = event.getHandle(sourceToken_);
    // HLT products may be registered in the input file yet absent in a given event.
    auto const size = source.isValid() ? source->size() : 0;
    hgcaldigi::HGCalDigiHost digis(size);
    auto detIds = std::vector<uint32_t>{};
    detIds.reserve(size);
    auto& view = digis.view();

    for (QIE10DigiCollection::size_type i = 0; i < size; ++i) {
      io_v1::QIE10DataFrame const frame((*source)[i]);
      detIds.push_back(frame.id());

      // The QIE10 SOI sample is the closest analogue of one HGCal channel row.
      // A frame without an SOI marker uses its first sample. Empty frames remain zero.
      int sampleIndex = frame.presamples();
      if (sampleIndex < 0)
        sampleIndex = 0;

      auto row = view[i];
      row.tctp() = 0;
      row.adcm1() = 0;
      row.adc() = 0;
      row.tot() = 0;
      row.toa() = 0;
      row.cm() = 0;
      // QIE10 status, trailing TDC, and CAPID do not have HGCal equivalents. Mark these
      // rows invalid so downstream HGCal reconstruction cannot treat them as digis.
      row.flags() = hgcal::DIGI_FLAG::Invalid;
      if (frame.samples() > 0) {
        auto const sample = frame[sampleIndex];
        row.adc() = sample.adc();
        row.toa() = sample.le_tdc();  // raw QIE10 TDC count, not calibrated HGCal ToA
        if (sampleIndex > 0)
          row.adcm1() = frame[sampleIndex - 1].adc();
      }
    }

    event.emplace(digisToken_, std::move(digis));
    event.emplace(detIdsToken_, std::move(detIds));
  }

  edm::EDGetTokenT<QIE10DigiCollection> const sourceToken_;
  edm::EDPutTokenT<hgcaldigi::HGCalDigiHost> const digisToken_;
  edm::EDPutTokenT<std::vector<uint32_t>> const detIdsToken_;
};

DEFINE_FWK_MODULE(QIE10ToHGCalDigiSoA);
