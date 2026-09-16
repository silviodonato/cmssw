#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

class HGCalUncalibRecHitCompressor : public edm::stream::EDProducer<> {
public:
  explicit HGCalUncalibRecHitCompressor(const edm::ParameterSet& configuration)
      : inputTags_(configuration.getParameter<std::vector<edm::InputTag>>("src")),
        geometryNames_(configuration.getParameter<std::vector<std::string>>("geometryNames")),
        tofDelays_(configuration.getParameter<std::vector<double>>("tofDelays")),
        toaLSBs_ns_(configuration.getParameter<std::vector<double>>("toaLSBs_ns")) {
    if (inputTags_.size() != geometryNames_.size()) {
      throw cms::Exception("Configuration")
          << "HGCalUncalibRecHitCompressor requires one geometryNames entry for each src entry";
    }
    if (inputTags_.size() != tofDelays_.size() || inputTags_.size() != toaLSBs_ns_.size()) {
      throw cms::Exception("Configuration")
          << "HGCalUncalibRecHitCompressor requires one tofDelay and toaLSB_ns entry for each src entry";
    }
    for (std::size_t index = 0; index < inputTags_.size(); ++index) {
      if (!std::isfinite(tofDelays_[index]) || !std::isfinite(toaLSBs_ns_[index]) || toaLSBs_ns_[index] <= 0.) {
        throw cms::Exception("Configuration") << "Invalid HGCal jitter encoding parameters for src entry " << index;
      }
    }
    inputTokens_.reserve(inputTags_.size());
    geometryTokens_.reserve(inputTags_.size());
    outputInstances_.reserve(inputTags_.size());

    for (std::size_t index = 0; index < inputTags_.size(); ++index) {
      const auto& inputTag = inputTags_[index];
      inputTokens_.push_back(consumes<HGCUncalibratedRecHitCollection>(inputTag));
      geometryTokens_.emplace_back(
          esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", geometryNames_[index]}));
      outputInstances_.push_back(inputTag.instance());
      produces<HGCUncalibratedRecHitCompressedCollection>(inputTag.instance());
    }
  }

  void produce(edm::Event& event, const edm::EventSetup& eventSetup) override {
    for (std::size_t index = 0; index < inputTokens_.size(); ++index) {
      const auto& input = event.get(inputTokens_[index]);
      const auto& activeDetIds = eventSetup.getData(geometryTokens_[index]).getValidDetIds();
      auto output = std::make_unique<HGCUncalibratedRecHitCompressedCollection>();
      output->reserve(input.size());
      output->setEncoding(HGCUncalibratedRecHitCompressedsSorted::Encoding::kGeometryIndexDelta);

      uint32_t previousGeometryIndex = 0;
      constexpr auto maxIndexDelta = std::numeric_limits<HGCUncalibratedRecHitCompressed::index_type>::max() - 1;
      for (const auto& hit : input) {
        const auto found = std::lower_bound(activeDetIds.begin(), activeDetIds.end(), hit.id());
        if (found == activeDetIds.end() || *found != hit.id()) {
          throw cms::Exception("LogicError") << "DetId 0x" << std::hex << hit.id().rawId() << std::dec
                                             << " is not valid in geometry " << geometryNames_[index];
        }
        const auto geometryIndex = static_cast<uint32_t>(std::distance(activeDetIds.begin(), found));
        if (geometryIndex < previousGeometryIndex) {
          throw cms::Exception("LogicError") << "HGCal geometry indices are not ordered in " << geometryNames_[index];
        }
        auto geometryIndexDelta = geometryIndex - previousGeometryIndex;
        while (geometryIndexDelta > maxIndexDelta) {
          output->push_back(HGCUncalibratedRecHitCompressed::makeIndexPadding(maxIndexDelta));
          geometryIndexDelta -= maxIndexDelta;
        }
        const auto compressedDelta = static_cast<HGCUncalibratedRecHitCompressed::index_type>(geometryIndexDelta);
        const HGCUncalibratedRecHitCompressed compressedHit(
            hit, compressedDelta, tofDelays_[index], toaLSBs_ns_[index]);
        output->push_back(compressedHit);

        constexpr double maxRelativeJitterDifference = 0.001;
        if (hit.jitter() > -99.f && hit.jitter() != 0.f) {
          const double originalJitter = hit.jitter();
          const double encodedJitter = compressedHit.jitter(tofDelays_[index], toaLSBs_ns_[index]);
          const double relativeDifference = std::abs(encodedJitter - originalJitter) / std::abs(originalJitter);
          if (relativeDifference > maxRelativeJitterDifference) {
            edm::LogWarning("HGCalUncalibRecHitCompressor")
                << "Jitter for DetId 0x" << std::hex << hit.id().rawId() << std::dec
                << " differs by " << 100. * relativeDifference << "% after compression (original: "
                << originalJitter << ", encoded: " << encodedJitter << ")";
          }
        }
        previousGeometryIndex = geometryIndex;
      }

      event.put(std::move(output), outputInstances_[index]);
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<std::vector<edm::InputTag>>("src",
                                                {edm::InputTag("hltHGCalUncalibRecHit", "HGCEEUncalibRecHits"),
                                                 edm::InputTag("hltHGCalUncalibRecHit", "HGCHEFUncalibRecHits"),
                                                 edm::InputTag("hltHGCalUncalibRecHit", "HGCHEBUncalibRecHits")});
    description.add<std::vector<std::string>>(
        "geometryNames", {"HGCalEESensitive", "HGCalHESiliconSensitive", "HGCalHEScintillatorSensitive"});
    description.add<std::vector<double>>("tofDelays");
    description.add<std::vector<double>>("toaLSBs_ns");
    descriptions.addWithDefaultLabel(description);
  }

private:
  std::vector<edm::InputTag> inputTags_;
  std::vector<std::string> geometryNames_;
  std::vector<double> tofDelays_;
  std::vector<double> toaLSBs_ns_;
  std::vector<edm::EDGetTokenT<HGCUncalibratedRecHitCollection>> inputTokens_;
  std::vector<edm::ESGetToken<HGCalGeometry, IdealGeometryRecord>> geometryTokens_;
  std::vector<std::string> outputInstances_;
};

DEFINE_FWK_MODULE(HGCalUncalibRecHitCompressor);
