#include <algorithm>
#include <array>
#include <atomic>
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
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDenseIndexMap.h"
#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalDigiInstances.h"

// Preserves exactly the input fields read by HGCalUncalibRecHitRecWeightsAlgo:
// DetId and sample 2's data, mode, threshold, ToA-valid flag and ToA.
// The standard SoA has no DetId column, so a row-aligned sidecar stores the electronics dense index
// of each DetId (see HGCalDenseIndexMap); DetIds absent from the map go to a second sidecar.
// Rows are sorted by dense index.
class HGCalDigisToRecHitSoA : public edm::global::EDProducer<edm::RunCache<hgcaldigi::HGCalDenseIndexMap>> {
public:
  explicit HGCalDigisToRecHitSoA(edm::ParameterSet const& config)
      : denseIndexInfoToken_(esConsumes<edm::Transition::BeginRun>()) {
    auto const source = config.getParameter<edm::InputTag>("src");
    for (std::size_t i = 0; i < hgcaldigi::digiInstances.size(); ++i) {
      auto const instance = hgcaldigi::digiInstances[i];
      sourceTokens_[i] = consumes<HGCalDigiCollection>(hgcaldigi::withInstance(source, instance));
      outputTokens_[i] = produces<hgcaldigi::HGCalDigiHost>(std::string(instance));
      denseIndicesTokens_[i] =
          produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "DenseIndices"));
      unmappedTokens_[i] = produces<std::vector<uint32_t>>(hgcaldigi::sidecarInstance(instance, "UnmappedDetIds"));
    }
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription description;
    description.add<edm::InputTag>("src", edm::InputTag("hltHgcalDigis"));
    descriptions.add("hltHgcalDigisSoA", description);
  }

  std::shared_ptr<hgcaldigi::HGCalDenseIndexMap> globalBeginRun(edm::Run const&,
                                                                edm::EventSetup const& setup) const override {
    auto map = std::make_shared<hgcaldigi::HGCalDenseIndexMap>(setup.getData(denseIndexInfoToken_));
    edm::LogInfo("HGCalDigisToRecHitSoA") << "Dense index map: " << map->size() << " indices, "
                                          << map->mappedDetIds() << " distinct z-minus DetIds";
    return map;
  }

  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

  void endJob() override {
    edm::LogVerbatim("HGCalDigisToRecHitSoA")
        << "Dense index encoding: " << zminus_ << " z-minus rows, " << zplus_ << " folded z-plus rows, "
        << unmapped_ << " unmapped rows";
  }

private:
  void produce(edm::StreamID, edm::Event& event, edm::EventSetup const&) const override {
    auto const& map = *runCache(event.getRun().index());
    for (std::size_t instanceIndex = 0; instanceIndex < hgcaldigi::digiInstances.size(); ++instanceIndex) {
      auto const source = event.getHandle(sourceTokens_[instanceIndex]);
      auto const size = source.isValid() ? source->size() : 0;
      hgcaldigi::HGCalDigiHost output(size);
      auto& view = output.view();
      std::vector<uint32_t> unmapped;

      // (dense index, input frame) pairs, so that rows can be written in ascending dense-index order:
      // the frame order is not needed (the decoder restores DetId order) and sorted indices compress better.
      std::vector<std::pair<uint32_t, HGCalDigiCollection::size_type>> order;
      order.reserve(size);
      for (HGCalDigiCollection::size_type i = 0; i < size; ++i) {
        auto const& frame = (*source)[i];
        if (frame.data().size() < 3) {
          throw cms::Exception("UnsupportedHGCalDigi")
              << "DetId " << frame.id().rawId() << " has fewer than three samples; the rec-hit algorithm reads sample 2";
        }

        auto const detId = frame.id().rawId();
        if (auto const index = map.index(detId)) {
          order.emplace_back(*index, i);
          if (*index < map.size())
            ++zminus_;
          else
            ++zplus_;
        } else {
          order.emplace_back(map.escapeBase() + unmapped.size(), i);
          unmapped.push_back(detId);
          ++unmapped_;
        }
      }
      std::sort(order.begin(), order.end());

      std::vector<uint32_t> denseIndices;
      denseIndices.reserve(size);
      for (std::size_t i = 0; i < order.size(); ++i) {
        auto const [index, frameIndex] = order[i];
        if (i > 0 && index == order[i - 1].first)
          throw cms::Exception("UnsupportedHGCalDigi") << "two frames share dense index " << index;
        denseIndices.push_back(index);

        auto const& inTime = (*source)[frameIndex].data()[2];
        auto row = view[i];
        // Private legacy-digi encoding within the standard SoA type:
        // tctp bits 0, 1, 2 carry mode, threshold, and ToA-valid respectively.
        // Rows are marked Invalid because these are not native ECON-D digis.
        row.tctp() = (inTime.mode() ? 1 : 0) | (inTime.threshold() ? 2 : 0) |
                     (inTime.getToAValid() ? 4 : 0);
        row.adcm1() = 0;
        row.adc() = inTime.mode() ? 0 : inTime.data();
        row.tot() = inTime.mode() ? inTime.data() : 0;
        row.toa() = inTime.getToAValid() ? inTime.toa() : 0;
        row.cm() = 0;
        row.flags() = hgcal::DIGI_FLAG::Invalid;
      }

      event.emplace(outputTokens_[instanceIndex], std::move(output));
      event.emplace(denseIndicesTokens_[instanceIndex], std::move(denseIndices));
      event.emplace(unmappedTokens_[instanceIndex], std::move(unmapped));
    }
  }

  edm::ESGetToken<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd> denseIndexInfoToken_;
  std::array<edm::EDGetTokenT<HGCalDigiCollection>, 3> sourceTokens_;
  std::array<edm::EDPutTokenT<hgcaldigi::HGCalDigiHost>, 3> outputTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> denseIndicesTokens_;
  std::array<edm::EDPutTokenT<std::vector<uint32_t>>, 3> unmappedTokens_;
  mutable std::atomic<std::size_t> zminus_ = 0, zplus_ = 0, unmapped_ = 0;
};

DEFINE_FWK_MODULE(HGCalDigisToRecHitSoA);
