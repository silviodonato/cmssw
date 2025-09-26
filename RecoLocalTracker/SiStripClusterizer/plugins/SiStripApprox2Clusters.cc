#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripApproximateClusterCollection.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "CalibFormats/SiStripObjects/interface/SiStripDetInfo.h"
#include "CalibTracker/SiStripCommon/interface/SiStripDetInfoFileReader.h"
#include "DataFormats/SiStripCommon/interface/ConstantsForHardwareSystems.h"


#include <vector>
#include <memory>

class SiStripApprox2Clusters : public edm::global::EDProducer<> {
public:
  explicit SiStripApprox2Clusters(const edm::ParameterSet& conf);

  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::EDGetTokenT<SiStripApproximateClusterCollection> clusterToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;
  SiStripDetInfo detInfo_;
};

SiStripApprox2Clusters::SiStripApprox2Clusters(const edm::ParameterSet& conf) {
  clusterToken_ = consumes(conf.getParameter<edm::InputTag>("inputApproxClusters"));
  tkGeomToken_ = esConsumes();
  detInfo_ = SiStripDetInfoFileReader::read(edm::FileInPath(SiStripDetInfoFileReader::kDefaultFile).fullPath());
  produces<edmNew::DetSetVector<SiStripCluster>>();
}

void SiStripApprox2Clusters::produce(edm::StreamID id, edm::Event& event, const edm::EventSetup& iSetup) const {
  auto result = std::make_unique<edmNew::DetSetVector<SiStripCluster>>();
  const auto& clusterCollection = event.get(clusterToken_);

  const auto& tkGeom = &iSetup.getData(tkGeomToken_);
  const auto& tkDets = tkGeom->dets();

  std::vector<uint16_t> v_strip;
  float previous_barycenter = 0;
  unsigned int offset_module_change = 0;

  unsigned int clusBegin = 0;

  for (const auto& detClusters : clusterCollection) {
    edmNew::DetSetVector<SiStripCluster>::FastFiller ff{*result, detClusters.id()};
    unsigned int detId = detClusters.id();

    uint16_t nStrips{0};
    auto det = std::find_if(tkDets.begin(), tkDets.end(), [detId](auto& elem) -> bool {
      return (elem->geographicalId().rawId() == detId);
    });
    const StripTopology& p = dynamic_cast<const StripGeomDetUnit*>(*det)->specificTopology();
    nStrips = p.nstrips() - 1;

    double nApvs = detInfo_.getNumberOfApvsAndStripLength(detId).first;

    for (const auto& cluster : detClusters) {
      const auto convertedCluster = SiStripCluster(cluster, nStrips, previous_barycenter, offset_module_change);
      if ((convertedCluster.barycenter()) >= nStrips + 1) {
        cms::Exception ex("DataCorrupt");
        ex << "SiStripApprox2Clusters: cluster with barycenter " << convertedCluster.barycenter()
           << " out of range for module with " << nStrips + 1 << " strips.";
        throw ex;
      }
      previous_barycenter = convertedCluster.barycenter();
      offset_module_change = 0;

      ++clusBegin;
      ff.push_back(convertedCluster);
    }
    offset_module_change = nApvs * sistrip::STRIPS_PER_APV;
  }

  event.put(std::move(result));
}

void SiStripApprox2Clusters::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("inputApproxClusters", edm::InputTag("siStripClusters"));
  descriptions.add("SiStripApprox2Clusters", desc);
}

DEFINE_FWK_MODULE(SiStripApprox2Clusters);
