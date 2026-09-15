#include <memory>
#include <string>
#include <vector>

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

class HGCalUncalibRecHitDecompressor : public edm::stream::EDProducer<> {
public:
  explicit HGCalUncalibRecHitDecompressor(const edm::ParameterSet& configuration)
      : inputTags_(configuration.getParameter<std::vector<edm::InputTag>>("src")),
        geometryNames_(configuration.getParameter<std::vector<std::string>>("geometryNames")) {
    if (inputTags_.size() != geometryNames_.size()) {
      throw cms::Exception("Configuration")
          << "HGCalUncalibRecHitDecompressor requires one geometryNames entry for each src entry";
    }
    inputTokens_.reserve(inputTags_.size());
    geometryTokens_.reserve(inputTags_.size());
    outputInstances_.reserve(inputTags_.size());

    for (std::size_t index = 0; index < inputTags_.size(); ++index) {
      const auto& inputTag = inputTags_[index];
      inputTokens_.push_back(consumes<HGCUncalibratedRecHitCompressedCollection>(inputTag));
      geometryTokens_.emplace_back(
          esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", geometryNames_[index]}));
      outputInstances_.push_back(inputTag.instance());
      produces<HGCUncalibratedRecHitCollection>(inputTag.instance());
    }
  }

  void produce(edm::Event& event, const edm::EventSetup& eventSetup) override {
    for (std::size_t index = 0; index < inputTokens_.size(); ++index) {
      const auto& input = event.get(inputTokens_[index]);
      const auto& activeDetIds = eventSetup.getData(geometryTokens_[index]).getValidDetIds();
      auto output = std::make_unique<HGCUncalibratedRecHitCollection>();
      output->reserve(input.size());

      if (!input.isGeometryIndexEncoded()) {
        throw cms::Exception("LogicError") << "Unsupported HGC uncalibrated-rechit compressed encoding";
      }
      input.forEachGeometryIndex([&](const auto& hit, uint32_t geometryIndex) {
        if (hit.amplitude() == 0.f) {
          return;
        }
        HGCUncalibratedRecHit decompressed(hit);
        if (geometryIndex >= activeDetIds.size()) {
          throw cms::Exception("LogicError")
              << "HGCal geometry index " << geometryIndex << " is outside " << geometryNames_[index]
              << " valid-DetId list of size " << activeDetIds.size();
        }
        decompressed.setId(activeDetIds[geometryIndex]);
        output->push_back(decompressed);
      });

      event.put(std::move(output), outputInstances_[index]);
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<std::vector<edm::InputTag>>(
        "src",
        {edm::InputTag("hltHGCalUncalibRecHitCompressed", "HGCEEUncalibRecHits"),
         edm::InputTag("hltHGCalUncalibRecHitCompressed", "HGCHEFUncalibRecHits"),
         edm::InputTag("hltHGCalUncalibRecHitCompressed", "HGCHEBUncalibRecHits")});
    description.add<std::vector<std::string>>(
        "geometryNames", {"HGCalEESensitive", "HGCalHESiliconSensitive", "HGCalHEScintillatorSensitive"});
    descriptions.addWithDefaultLabel(description);
  }

private:
  std::vector<edm::InputTag> inputTags_;
  std::vector<std::string> geometryNames_;
  std::vector<edm::EDGetTokenT<HGCUncalibratedRecHitCompressedCollection>> inputTokens_;
  std::vector<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>> geometryTokens_;
  std::vector<std::string> outputInstances_;
};

DEFINE_FWK_MODULE(HGCalUncalibRecHitDecompressor);
