#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"

#include "TH1F.h"

class TrackPropSimple : public edm::one::EDAnalyzer<> {
public:
  explicit TrackPropSimple(const edm::ParameterSet&);
  ~TrackPropSimple() override = default;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // Tokens for getting data from the event and event setup
  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> propagatorToken_;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttbToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;

  // Counter variables for propagation fraction
  long long totalClustersMatched_ = 0;
  long long failedPropagations_ = 0;

  // Histogram
  TH1F* h_dr_strip;
};

TrackPropSimple::TrackPropSimple(const edm::ParameterSet& iConfig)
    : tracksToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("tracks"))),
      propagatorToken_(esConsumes(edm::ESInputTag("", "PropagatorWithMaterialParabolicMf"))),
      ttbToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
      tkGeomToken_(esConsumes()) {
  
  // Initialize TFileService and histogram
  edm::Service<TFileService> fs;
  h_dr_strip = fs->make<TH1F>("h_dr_strip", "Distance between propagated track and cluster barycenter;dr [cm];Hits", 100, 0, 0.5);
}

void TrackPropSimple::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // 1. Get the tracks from the event
  auto const& tracks = iEvent.get(tracksToken_);

  // 2. Get the tools from the Event Setup
  auto const& thePropagator = iSetup.getData(propagatorToken_);
  auto const& theTTrackBuilder = iSetup.getData(ttbToken_);
  auto const& tkGeom = iSetup.getData(tkGeomToken_);

  // Clone propagator to allow AnyDirection (useful for reaching hits anywhere on the track)
  std::unique_ptr<Propagator> anyProp(thePropagator.clone());
  anyProp->setPropagationDirection(anyDirection);

  for (const auto& trk : tracks) {
    // if (trk.pt() < 1.0) continue; // Basic quality cut

    // Build a TransientTrack (lets us compute states easily)
    reco::TransientTrack tkTT = theTTrackBuilder.build(trk);
    // if (!tkTT.impactPointState().isValid()) continue;

    // Loop over the hits (RecHits) on the track
    for (auto const& hit : trk.recHits()) {
      if (!hit->isValid()) continue;

      // We are looking for Strip clusters. 
      // Matched hits have components. Single hits have none (or return themselves).
      std::vector<const TrackingRecHit*> componentHits;
      auto const& hits = hit->recHits();
      if (hits.empty()) {
        componentHits.push_back(hit);
      } else {
        for (auto const& h : hits) componentHits.push_back(h);
      }

      for (const auto* cHit : componentHits) {
        // if (!cHit || !cHit->isValid()) continue;
        DetId detId = cHit->geographicalId();
        
        // Process only Strip detectors (TIB, TID, TOB, TEC)
        if (detId.det() != DetId::Tracker) continue;
        int subdet = detId.subdetId();
        if (subdet < 3) continue; // 1,2 are Pixels

        const GeomDet* geomDet = tkGeom.idToDet(detId);
        const StripGeomDetUnit* stripDet = dynamic_cast<const StripGeomDetUnit*>(geomDet);
        if (!stripDet) continue;

        // Try to get the cluster associated with this hit
        const SiStripCluster* cluster = nullptr;
        if (const auto* h2D = dynamic_cast<const SiStripRecHit2D*>(cHit)) {
           if (h2D->cluster().isAvailable()) cluster = &*h2D->cluster();
        } else if (const auto* h1D = dynamic_cast<const SiStripRecHit1D*>(cHit)) {
           if (h1D->cluster().isAvailable()) cluster = &*h1D->cluster();
        }
        if (!cluster) continue;

        totalClustersMatched_++;

        // 3. Propagate the track to the DetId surface
        // We propagate from the Impact Point (Beam Spot) to the module
        TrajectoryStateOnSurface tsos = anyProp->propagate(tkTT.innermostMeasurementState(), geomDet->surface());

        double dr = 0.05; // Default "fake" value if propagation fails
        if (tsos.isValid()) {
          // Get the coordinates
          auto const& topology = stripDet->specificTopology();
          float barycenter = cluster->barycenter();
          
          // Predicted local X from track vs Measured local X from cluster barycenter
          double x_track = tsos.localPosition().x();
          double x_cluster = topology.localPosition(barycenter).x();
          
          dr = std::abs(x_track - x_cluster);
        } else {
          failedPropagations_++;
        }

        h_dr_strip->Fill(dr);
      }
    }
  }
}

void TrackPropSimple::endJob() {
  double failureFraction = 0;
  if (totalClustersMatched_ > 0) {
    failureFraction = (double)failedPropagations_ / totalClustersMatched_;
  }

  std::cout << "\n--- TrackPropSimple Summary ---" << std::endl;
  std::cout << "Total Strip Clusters checked: " << totalClustersMatched_ << std::endl;
  if (totalClustersMatched_ > 0) {
    std::cout << "Failed propagations:        " << failedPropagations_ 
              << " (" << (failureFraction * 100.0) << "%)" << std::endl;
  }
  std::cout << "Residual dist Mean:         " << h_dr_strip->GetMean() << " cm" << std::endl;
  std::cout << "--------------------------------\n" << std::endl;
}

DEFINE_FWK_MODULE(TrackPropSimple);
