#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TH2.h"
#include "TTree.h"

#include <cstdint>
#include <string>
#include <vector>

class HGCalDigiNtuplizer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit HGCalDigiNtuplizer(edm::ParameterSet const& config)
      : digisToken_(consumes<HGCalDigiCollection>(config.getParameter<edm::InputTag>("digis"))) {
    usesResource(TFileService::kSharedResource);
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("digis", edm::InputTag("hltHgcalDigisDecompressed", "EE"));
    descriptions.add("hgcalDigiNtuplizer", description);
  }

private:
  void beginJob() override;
  void analyze(edm::Event const&, edm::EventSetup const&) override;

  edm::EDGetTokenT<HGCalDigiCollection> digisToken_;
  TTree* tree_ = nullptr;

  uint32_t run_ = 0, lumi_ = 0, detid_ = 0, raw_ = 0;
  uint64_t event_ = 0;
  int32_t zside_ = 0, layer_ = 0, waferType_ = 0, waferU_ = 0, waferV_ = 0, cellU_ = 0, cellV_ = 0;
  int32_t sample_ = 0, data_ = 0, adc_ = -1, tot_ = -1, toa_ = 0, gain_ = 0;
  bool threshold_ = false, mode_ = false, toaValid_ = false;

  TH2I *dataToa_ = nullptr, *dataGain_ = nullptr, *dataSample_ = nullptr, *dataLayer_ = nullptr;
  TH2I *toaGain_ = nullptr, *toaSample_ = nullptr, *toaLayer_ = nullptr;
  TH2I *gainSample_ = nullptr, *gainLayer_ = nullptr, *sampleLayer_ = nullptr;
  TH2I *adcToa_ = nullptr, *adcGain_ = nullptr, *adcSample_ = nullptr;
  TH2I *totToa_ = nullptr, *totGain_ = nullptr, *totSample_ = nullptr;
};

void HGCalDigiNtuplizer::beginJob() {
  edm::Service<TFileService> file;
  tree_ = file->make<TTree>("digis", "One row per EE digi sample");
  tree_->Branch("run", &run_, "run/i");
  tree_->Branch("lumi", &lumi_, "lumi/i");
  tree_->Branch("event", &event_, "event/l");
  tree_->Branch("detid", &detid_, "detid/i");
  tree_->Branch("zside", &zside_, "zside/I");
  tree_->Branch("layer", &layer_, "layer/I");
  tree_->Branch("waferType", &waferType_, "waferType/I");
  tree_->Branch("waferU", &waferU_, "waferU/I");
  tree_->Branch("waferV", &waferV_, "waferV/I");
  tree_->Branch("cellU", &cellU_, "cellU/I");
  tree_->Branch("cellV", &cellV_, "cellV/I");
  tree_->Branch("sample", &sample_, "sample/I");
  tree_->Branch("raw", &raw_, "raw/i");
  tree_->Branch("data", &data_, "data/I");
  tree_->Branch("adc", &adc_, "adc/I");
  tree_->Branch("tot", &tot_, "tot/I");
  tree_->Branch("toa", &toa_, "toa/I");
  tree_->Branch("gain", &gain_, "gain/I");
  tree_->Branch("threshold", &threshold_, "threshold/O");
  tree_->Branch("mode", &mode_, "mode/O");
  tree_->Branch("toaValid", &toaValid_, "toaValid/O");

  auto make = [&file](char const* name, char const* title, int nx, double xmin, double xmax,
                      int ny, double ymin, double ymax) {
    return file->make<TH2I>(name, title, nx, xmin, xmax, ny, ymin, ymax);
  };
  dataToa_ = make("data_vs_toa", "Data vs TOA (valid TOA);data (ADC or TOT);TOA", 256, 0, 4096, 256, 0, 1024);
  dataGain_ = make("data_vs_gain", "Data vs gain;data (ADC or TOT);gain", 256, 0, 4096, 16, 0, 16);
  dataSample_ = make("data_vs_sample", "Data vs sample;data (ADC or TOT);sample", 256, 0, 4096, 15, 0, 15);
  dataLayer_ = make("data_vs_layer", "Data vs layer;data (ADC or TOT);layer", 256, 0, 4096, 32, 0, 32);
  toaGain_ = make("toa_vs_gain", "TOA vs gain (valid TOA);TOA;gain", 256, 0, 1024, 16, 0, 16);
  toaSample_ = make("toa_vs_sample", "TOA vs sample (valid TOA);TOA;sample", 256, 0, 1024, 15, 0, 15);
  toaLayer_ = make("toa_vs_layer", "TOA vs layer (valid TOA);TOA;layer", 256, 0, 1024, 32, 0, 32);
  gainSample_ = make("gain_vs_sample", "Gain vs sample;gain;sample", 16, 0, 16, 15, 0, 15);
  gainLayer_ = make("gain_vs_layer", "Gain vs layer;gain;layer", 16, 0, 16, 32, 0, 32);
  sampleLayer_ = make("sample_vs_layer", "Sample vs layer;sample;layer", 15, 0, 15, 32, 0, 32);
  adcToa_ = make("adc_vs_toa", "ADC vs TOA (ADC mode, valid TOA);ADC;TOA", 256, 0, 4096, 256, 0, 1024);
  adcGain_ = make("adc_vs_gain", "ADC vs gain (ADC mode);ADC;gain", 256, 0, 4096, 16, 0, 16);
  adcSample_ = make("adc_vs_sample", "ADC vs sample (ADC mode);ADC;sample", 256, 0, 4096, 15, 0, 15);
  totToa_ = make("tot_vs_toa", "TOT vs TOA (TOT mode, valid TOA);TOT;TOA", 256, 0, 4096, 256, 0, 1024);
  totGain_ = make("tot_vs_gain", "TOT vs gain (TOT mode);TOT;gain", 256, 0, 4096, 16, 0, 16);
  totSample_ = make("tot_vs_sample", "TOT vs sample (TOT mode);TOT;sample", 256, 0, 4096, 15, 0, 15);
}

void HGCalDigiNtuplizer::analyze(edm::Event const& event, edm::EventSetup const&) {
  auto const& digis = event.get(digisToken_);
  run_ = event.id().run();
  lumi_ = event.id().luminosityBlock();
  event_ = event.id().event();

  for (auto const& frame : digis) {
    HGCSiliconDetId const id(frame.id());
    detid_ = id.rawId();
    zside_ = id.zside();
    layer_ = id.layer();
    waferType_ = id.type();
    waferU_ = id.waferU();
    waferV_ = id.waferV();
    cellU_ = id.cellU();
    cellV_ = id.cellV();

    for (sample_ = 0; sample_ < frame.size(); ++sample_) {
      auto const& value = frame.sample(sample_);
      raw_ = value.raw();
      data_ = value.data();
      mode_ = value.mode();
      adc_ = mode_ ? -1 : data_;
      tot_ = mode_ ? data_ : -1;
      toa_ = value.toa();
      gain_ = value.gain();
      threshold_ = value.threshold();
      toaValid_ = value.getToAValid();
      tree_->Fill();

      dataGain_->Fill(data_, gain_);
      dataSample_->Fill(data_, sample_);
      dataLayer_->Fill(data_, layer_);
      gainSample_->Fill(gain_, sample_);
      gainLayer_->Fill(gain_, layer_);
      sampleLayer_->Fill(sample_, layer_);
      if (toaValid_) {
        dataToa_->Fill(data_, toa_);
        toaGain_->Fill(toa_, gain_);
        toaSample_->Fill(toa_, sample_);
        toaLayer_->Fill(toa_, layer_);
      }
      if (mode_) {
        totGain_->Fill(tot_, gain_);
        totSample_->Fill(tot_, sample_);
        if (toaValid_)
          totToa_->Fill(tot_, toa_);
      } else {
        adcGain_->Fill(adc_, gain_);
        adcSample_->Fill(adc_, sample_);
        if (toaValid_)
          adcToa_->Fill(adc_, toa_);
      }
    }
  }
}

DEFINE_FWK_MODULE(HGCalDigiNtuplizer);
