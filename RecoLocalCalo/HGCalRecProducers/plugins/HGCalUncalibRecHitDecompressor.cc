#include <memory>
#include <string>
#include <vector>

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

class HGCalUncalibRecHitDecompressor : public edm::stream::EDProducer<> {
public:
  explicit HGCalUncalibRecHitDecompressor(const edm::ParameterSet& configuration)
      : inputTags_(configuration.getParameter<std::vector<edm::InputTag>>("src")) {
    inputTokens_.reserve(inputTags_.size());
    outputInstances_.reserve(inputTags_.size());

    for (const auto& inputTag : inputTags_) {
      inputTokens_.push_back(consumes<HGCUncalibratedRecHitCompressedCollection>(inputTag));
      outputInstances_.push_back(inputTag.instance());
      produces<HGCUncalibratedRecHitCollection>(inputTag.instance());
    }
  }

  void produce(edm::Event& event, const edm::EventSetup&) override {
    for (std::size_t index = 0; index < inputTokens_.size(); ++index) {
      const auto& input = event.get(inputTokens_[index]);
      auto output = std::make_unique<HGCUncalibratedRecHitCollection>();
      output->reserve(input.size());

      for (const auto& hit : input)
        output->push_back(HGCUncalibratedRecHit(hit));

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
    descriptions.addWithDefaultLabel(description);
  }

private:
  std::vector<edm::InputTag> inputTags_;
  std::vector<edm::EDGetTokenT<HGCUncalibratedRecHitCompressedCollection>> inputTokens_;
  std::vector<std::string> outputInstances_;
};

DEFINE_FWK_MODULE(HGCalUncalibRecHitDecompressor);
