#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "TrackingTools/TransientTrackingRecHit/interface/GenericTransientTrackingRecHit.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TValidTrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/StripClusterParameterEstimator.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "TrackingTools/TrajectoryParametrization/interface/LocalTrajectoryError.h"
#include "TH1F.h"

#include "TCanvas.h"
#include "TLegend.h"

class TrackPropAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit TrackPropAnalyzer(const edm::ParameterSet&);
  ~TrackPropAnalyzer() override = default;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;
  void processHit(TransientTrackingRecHit::ConstRecHitPointer hit,
                  const reco::TransientTrack& tkTT,
                  const Propagator& thePropagator,
                  const TrackerGeometry& tkGeom,
                  const StripClusterParameterEstimator* stripCPE,
                  const PixelClusterParameterEstimator* pixelCPE,
                  const TrajectoryStateOnSurface& tsosInnerSeed,
                  const TrajectoryStateOnSurface& tsosOuterPixelSeed,
                  const TransientTrackingRecHit::ConstRecHitContainer& trackHits);

  TrajectoryStateOnSurface getCorrectedState(const TrajectoryStateOnSurface& state, 
                                           TransientTrackingRecHit::ConstRecHitPointer hit,
                                           const StripClusterParameterEstimator* stripCPE,
                                           const PixelClusterParameterEstimator* pixelCPE) {
    if (!state.isValid() || !hit || !hit->isValid() || !hit->hit()) return state;

    DetId detId = hit->geographicalId();
    if (detId.det() != DetId::Tracker) return state;

    // For generalTracks RECO, clusters are often missing. Check availability.
    const TrackingRecHit* rawHit = hit->hit();
    
    // Pixel Correction (only if requested and available)
    if (detId.subdetId() == (int)PixelSubdetector::PixelBarrel || 
        detId.subdetId() == (int)PixelSubdetector::PixelEndcap) {
      if (pixelCPE) {
        if (const auto* pixHit = dynamic_cast<const SiPixelRecHit*>(rawHit)) {
          if (pixHit->cluster().isAvailable() && pixHit->cluster().isNonnull()) {
            const PixelGeomDetUnit* pixDet = dynamic_cast<const PixelGeomDetUnit*>(hit->det());
            if (pixDet) {
              TrajectoryStateOnSurface nonConstState = state;
              auto localValues = pixelCPE->localParametersV(*pixHit->cluster(), *pixDet, nonConstState);
              if (!localValues.empty()) {
                return TrajectoryStateOnSurface(
                    LocalTrajectoryParameters(localValues[0].first, state.localMomentum(), state.charge()),
                    state.localError(), state.surface(), &state.globalParameters().magneticField());
              }
            }
          }
        }
      }
      return state;
    }

    // Strip Correction
    if (stripCPE) {
      const SiStripCluster* cluster = nullptr;
      if (const auto* hit2D = dynamic_cast<const SiStripRecHit2D*>(rawHit)) {
        if (hit2D->cluster().isAvailable() && hit2D->cluster().isNonnull()) cluster = &*hit2D->cluster();
      } else if (const auto* hit1D = dynamic_cast<const SiStripRecHit1D*>(rawHit)) {
        if (hit1D->cluster().isAvailable() && hit1D->cluster().isNonnull()) cluster = &*hit1D->cluster();
      }

      if (cluster) {
        const StripGeomDetUnit* stripDet = dynamic_cast<const StripGeomDetUnit*>(hit->det());
        if (stripDet) {
          auto localValues = stripCPE->localParameters(*cluster, *stripDet, state);
          return TrajectoryStateOnSurface(
              LocalTrajectoryParameters(localValues.first, state.localMomentum(), state.charge()),
              state.localError(), state.surface(), &state.globalParameters().magneticField());
        }
      }
    }
    return state;
  }

  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> propagatorToken_;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttbToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;
  edm::ESGetToken<StripClusterParameterEstimator, TkStripCPERecord> stripCPEToken_;
  edm::ESGetToken<PixelClusterParameterEstimator, TkPixelCPERecord> pixelCPEToken_;
  edm::ESGetToken<TransientTrackingRecHitBuilder, TransientRecHitRecord> ttrhBuilderToken_;

  long long totalHits_ = 0;
  long long totalStripHits_ = 0;
  long long totalPixelHits_ = 0;
  long long validInner_ = 0;
  long long validImpact_ = 0;
  long long validOuter_ = 0;
  long long validOuterPixel_ = 0;
  long long validClosest_ = 0;
  long long validInnerOrOuter_ = 0;
  long long validImpactOrOuter_ = 0;
  long long validInnerOrImpact_ = 0;
  long long validAllOr_ = 0;
  
  TH1F* h_dr_inner;
  TH1F* h_dr_inner_reco;
  TH1F* h_dr_pixel_dx;
  TH1F* h_dr_pixel_dy;
  TH1F* h_dr_impact;
  TH1F* h_dr_outer;
  TH1F* h_dr_outerPixel;
  TH1F* h_dr_closest;
  TH1F* h_pt;
};

TrackPropAnalyzer::TrackPropAnalyzer(const edm::ParameterSet& iConfig)
    : tracksToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("tracks"))),
      propagatorToken_(esConsumes(edm::ESInputTag("", "PropagatorWithMaterialParabolicMf"))),
      ttbToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
      tkGeomToken_(esConsumes()),
      stripCPEToken_(esConsumes(edm::ESInputTag("", "StripCPEfromTrackAngle"))),
      pixelCPEToken_(esConsumes(edm::ESInputTag("", "PixelCPEGeneric"))),
      ttrhBuilderToken_(esConsumes(edm::ESInputTag("", "WithTrackAngle"))) {
  edm::Service<TFileService> fs;
  h_dr_inner = fs->make<TH1F>("h_dr_inner", "dr (Innermost);dr [cm];Hits", 100, 0, 0.5);
  h_dr_inner_reco = fs->make<TH1F>("h_dr_inner_reco", "dr RECO (Innermost);dr [cm];Hits", 100, 0, 0.5);
  h_dr_pixel_dx = fs->make<TH1F>("h_dr_pixel_dx", "Pixel dx;dx [cm];Hits", 100, 0, 0.1);
  h_dr_pixel_dy = fs->make<TH1F>("h_dr_pixel_dy", "Pixel dy;dy [cm];Hits", 100, 0, 0.1);
  h_dr_impact = fs->make<TH1F>("h_dr_impact", "dr (Impact Point);dr [cm];Hits", 100, 0, 0.5);
  h_dr_outer = fs->make<TH1F>("h_dr_outer", "dr (Outermost);dr [cm];Hits", 100, 0, 0.5);
  h_dr_outerPixel = fs->make<TH1F>("h_dr_outerPixel", "dr (Outer Pixel);dr [cm];Hits", 100, 0, 0.5);
  h_dr_closest = fs->make<TH1F>("h_dr_closest", "dr (Closest Hit);dr [cm];Hits", 100, 0, 0.5);
  h_pt = fs->make<TH1F>("h_pt", "Track pT;pT [GeV];Tracks", 100, 0, 50);

  h_dr_inner->Sumw2();
  h_dr_inner_reco->Sumw2();
  h_dr_pixel_dx->Sumw2();
  h_dr_pixel_dy->Sumw2();
  h_dr_impact->Sumw2();
  h_dr_outer->Sumw2();
  h_dr_outerPixel->Sumw2();
  h_dr_closest->Sumw2();
}

void TrackPropAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  auto const& tracks = iEvent.get(tracksToken_);
  auto const& thePropagator = iSetup.getData(propagatorToken_);
  auto const& theTTrackBuilder = iSetup.getData(ttbToken_);
  auto const& tkGeom = iSetup.getData(tkGeomToken_);
  auto const& stripCPE = iSetup.getData(stripCPEToken_);
  auto const& pixelCPE = iSetup.getData(pixelCPEToken_);

  // Clone propagator to allow 'anyDirection' for backward propagation
  std::unique_ptr<Propagator> anyDirectionPropagator(thePropagator.clone());
  anyDirectionPropagator->setPropagationDirection(anyDirection);

  for (const auto& trk : tracks) {
    if (trk.pt() < 1.0) continue;
    h_pt->Fill(trk.pt());

    reco::TransientTrack tkTT = theTTrackBuilder.build(trk);
    if (!tkTT.innermostMeasurementState().isValid()) continue;

    TransientTrackingRecHit::ConstRecHitContainer trackHits;
    for (auto const& hit : trk.recHits()) {
      if (!hit->isValid()) continue;
      
      DetId detId = hit->geographicalId();
      const GeomDet* gd = tkGeom.idToDet(detId);
      if (!gd) continue;

      const SiStripMatchedRecHit2D* matchedHit = dynamic_cast<const SiStripMatchedRecHit2D*>(hit);
      if (matchedHit) {
        const SiStripRecHit2D& mono = matchedHit->monoHit();
        const SiStripRecHit2D& stereo = matchedHit->stereoHit();
        
        if (mono.isValid()) {
          const GeomDet* gdm = tkGeom.idToDet(mono.geographicalId());
          if (gdm) trackHits.push_back(GenericTransientTrackingRecHit::build(gdm, &mono));
        }
        if (stereo.isValid()) {
          const GeomDet* gds = tkGeom.idToDet(stereo.geographicalId());
          if (gds) trackHits.push_back(GenericTransientTrackingRecHit::build(gds, &stereo));
        }
      } else {
        trackHits.push_back(GenericTransientTrackingRecHit::build(gd, hit));
      }
    }

    if (trackHits.empty()) continue;

    TransientTrackingRecHit::ConstRecHitPointer hitInner = trackHits.front();
    TransientTrackingRecHit::ConstRecHitPointer hitOuterPixel = nullptr;
    for (auto const& thit : trackHits) {
      DetId detId = thit->geographicalId();
      if (detId.det() == DetId::Tracker && 
         (detId.subdetId() == PixelSubdetector::PixelBarrel || detId.subdetId() == PixelSubdetector::PixelEndcap)) {
        hitOuterPixel = thit;
      }
    }

    TrajectoryStateOnSurface tsosInnerSeed;
    if (hitInner && hitInner->isValid() && hitInner->det() && tkTT.innermostMeasurementState().isValid()) {
      tsosInnerSeed = getCorrectedState(tkTT.innermostMeasurementState(), hitInner, &stripCPE, &pixelCPE);
    }

    TrajectoryStateOnSurface tsosOuterPixelSeed;
    if (hitOuterPixel && hitOuterPixel->isValid() && hitOuterPixel->det() && tkTT.impactPointState().isValid()) {
      auto tsosAtOP = anyDirectionPropagator->propagate(tkTT.impactPointState(), hitOuterPixel->det()->surface());
      if (tsosAtOP.isValid()) {
        tsosOuterPixelSeed = getCorrectedState(tsosAtOP, hitOuterPixel, &stripCPE, &pixelCPE);
      }
    }

    for (auto const& tHit : trackHits) {
      if (tHit && tHit->det()) {
         processHit(tHit, tkTT, *anyDirectionPropagator, tkGeom, &stripCPE, &pixelCPE, tsosInnerSeed, tsosOuterPixelSeed, trackHits);
      }
    }
  }
}

void TrackPropAnalyzer::processHit(TransientTrackingRecHit::ConstRecHitPointer hit,
                                  const reco::TransientTrack& tkTT,
                                  const Propagator& thePropagator,
                                  const TrackerGeometry& tkGeom,
                                  const StripClusterParameterEstimator* stripCPE,
                                  const PixelClusterParameterEstimator* pixelCPE,
                                  const TrajectoryStateOnSurface& tsosInnerSeed,
                                  const TrajectoryStateOnSurface& tsosOuterPixelSeed,
                                  const TransientTrackingRecHit::ConstRecHitContainer& trackHits) {
  if (!hit || !hit->isValid() || !hit->det()) return;
  DetId detId = hit->geographicalId();
  if (detId.det() != DetId::Tracker) return;

  // Skip Pixels for residuals
  if (detId.subdetId() == (int)PixelSubdetector::PixelBarrel || 
      detId.subdetId() == (int)PixelSubdetector::PixelEndcap) return;

  const GeomDet* geomDet = hit->det();
  totalHits_++;
  
  TrajectoryStateOnSurface tsosRef = thePropagator.propagate(tkTT.impactPointState(), geomDet->surface());
  if (!tsosRef.isValid()) return;

  // Get raw cluster to use barycenter (RECO ground truth)
  const SiStripCluster* targetCluster = nullptr;
  auto const* transientHit = hit->hit();
  if (!transientHit) return;

  if (const auto* h2 = dynamic_cast<const SiStripRecHit2D*>(transientHit)) {
    if (h2->cluster().isAvailable() && h2->cluster().isNonnull()) targetCluster = &*h2->cluster();
  } else if (const auto* h1 = dynamic_cast<const SiStripRecHit1D*>(transientHit)) {
    if (h1->cluster().isAvailable() && h1->cluster().isNonnull()) targetCluster = &*h1->cluster();
  }
  if (!targetCluster) return;

  auto stripDet = dynamic_cast<const StripGeomDetUnit*>(geomDet);
  if (!stripDet) return;

  auto const& topology = stripDet->specificTopology();
  float barycenter = targetCluster->barycenter();
  double target_x = topology.localPosition(barycenter).x();

  totalStripHits_++;
  
  // RECO distance (original track prediction)
  auto tsosReco = tkTT.stateOnSurface(geomDet->position());
  if (tsosReco.isValid()) {
    h_dr_inner_reco->Fill(std::abs(tsosReco.localPosition().x() - target_x));
  }

  // 1. Propagation FROM Innermost Hit
  TrajectoryStateOnSurface tsosInnerComp;
  if (tsosInnerSeed.isValid()) {
    tsosInnerComp = thePropagator.propagate(tsosInnerSeed, geomDet->surface());
    if (tsosInnerComp.isValid()) {
      h_dr_inner->Fill(std::abs(tsosInnerComp.localPosition().x() - target_x));
      validInner_++;
    }
  }

  // 2. Propagation FROM Impact Point
  if (tsosRef.isValid()) {
    h_dr_impact->Fill(std::abs(tsosRef.localPosition().x() - target_x));
    validImpact_++;
  }
  
  // 3. Propagation FROM Outermost Hit
  TrajectoryStateOnSurface tsosOuter = thePropagator.propagate(tkTT.outermostMeasurementState(), geomDet->surface());
  if (tsosOuter.isValid()) {
    h_dr_outer->Fill(std::abs(tsosOuter.localPosition().x() - target_x));
    validOuter_++;
  }

  // 4. Propagation FROM Outermost Pixel Hit
  if (tsosOuterPixelSeed.isValid()) {
    TrajectoryStateOnSurface tsosOP = thePropagator.propagate(tsosOuterPixelSeed, geomDet->surface());
    if (tsosOP.isValid()) {
      h_dr_outerPixel->Fill(std::abs(tsosOP.localPosition().x() - target_x));
      validOuterPixel_++;
    }
  }

  // 5. Propagation FROM Closest Hit
  double minDistance = 99999.0;
  GlobalPoint targetPosGlobal = geomDet->position();
  TransientTrackingRecHit::ConstRecHitPointer closestHit;

  for (auto const& otherHit : trackHits) {
    if (otherHit->geographicalId() == detId) continue;
    if (!otherHit->isValid() || !otherHit->det()) continue;
    double dist = (otherHit->det()->position() - targetPosGlobal).mag();
    if (dist < minDistance) {
      minDistance = dist;
      closestHit = otherHit;
    }
  }

  if (closestHit) {
    auto tsosAtClosest = tkTT.stateOnSurface(closestHit->det()->position());
    if (tsosAtClosest.isValid()) {
      auto tsosClCorr = getCorrectedState(tsosAtClosest, closestHit, stripCPE, pixelCPE);
      auto tsosClosestProp = thePropagator.propagate(tsosClCorr, geomDet->surface());
      if (tsosClosestProp.isValid()) {
        h_dr_closest->Fill(std::abs(tsosClosestProp.localPosition().x() - target_x));
        validClosest_++;
      }
    }
  }

  if (tsosInnerComp.isValid() || tsosOuter.isValid()) validInnerOrOuter_++;
  if (tsosRef.isValid() || tsosOuter.isValid()) validImpactOrOuter_++;
  if (tsosInnerComp.isValid() || tsosRef.isValid()) validInnerOrImpact_++;
  if (tsosInnerComp.isValid() || tsosRef.isValid() || tsosOuter.isValid()) validAllOr_++;
}

void TrackPropAnalyzer::endJob() {
  double fInner = (totalHits_ > 0) ? (double)validInner_ / totalHits_ : 0;
  double fImpact = (totalHits_ > 0) ? (double)validImpact_ / totalHits_ : 0;
  double fOuter = (totalHits_ > 0) ? (double)validOuter_ / totalHits_ : 0;
  double fOuterPixel = (totalHits_ > 0) ? (double)validOuterPixel_ / totalHits_ : 0;
  double fClosest = (totalHits_ > 0) ? (double)validClosest_ / totalHits_ : 0;
  double fInnerOuter = (totalHits_ > 0) ? (double)validInnerOrOuter_ / totalHits_ : 0;
  double fImpactOuter = (totalHits_ > 0) ? (double)validImpactOrOuter_ / totalHits_ : 0;
  double fInnerImpact = (totalHits_ > 0) ? (double)validInnerOrImpact_ / totalHits_ : 0;
  double fAll = (totalHits_ > 0) ? (double)validAllOr_ / totalHits_ : 0;

  std::cout << "\n--- Track Propagation Summary (per hit) ---" << std::endl;
  std::cout << "Total Valid Hits checked: " << totalHits_ << " (Strip: " << totalStripHits_ << ", Pixel: " << totalPixelHits_ << ")" << std::endl;
  std::cout << "1) Innermost:      " << validInner_ << " (" << fInner << ")" << std::endl;
  std::cout << "Mean h_dr_inner:   " << h_dr_inner->GetMean() << " cm" << std::endl;
  std::cout << "Mean h_dr_inner_reco: " << h_dr_inner_reco->GetMean() << " cm (raw RECO distance)" << std::endl;
  std::cout << "Mean Pixel dx/dy:  " << h_dr_pixel_dx->GetMean() << " / " << h_dr_pixel_dy->GetMean() << " cm" << std::endl;
  std::cout << "2) Impact Point:   " << validImpact_ << " (" << fImpact << ")" << std::endl;
  std::cout << "3) Outermost:      " << validOuter_ << " (" << fOuter << ")" << std::endl;
  std::cout << "4) Outermost Pixel:" << validOuterPixel_ << " (" << fOuterPixel << ")" << std::endl;
  std::cout << "5) Closest Hit:    " << validClosest_ << " (" << fClosest << ")" << std::endl;
  std::cout << "1 OR 2:            " << validInnerOrImpact_ << " (" << fInnerImpact << ")" << std::endl;
  std::cout << "1 OR 3:            " << validInnerOrOuter_ << " (" << fInnerOuter << ")" << std::endl;
  std::cout << "2 OR 3:            " << validImpactOrOuter_ << " (" << fImpactOuter << ")" << std::endl;
  std::cout << "1 OR 2 OR 3:       " << validAllOr_ << " (" << fAll << ")" << std::endl;
  std::cout << "-------------------------------------------\n" << std::endl;

  TCanvas *c = new TCanvas("c", "dr distribution (normalized)", 1000, 800);
  c->SetGrid();
  
  if (h_dr_inner->Integral() > 0) h_dr_inner->Scale(1.0/h_dr_inner->Integral());
  if (h_dr_inner_reco->Integral() > 0) h_dr_inner_reco->Scale(1.0/h_dr_inner_reco->Integral());
  if (h_dr_pixel_dx->Integral() > 0) h_dr_pixel_dx->Scale(1.0/h_dr_pixel_dx->Integral());
  if (h_dr_pixel_dy->Integral() > 0) h_dr_pixel_dy->Scale(1.0/h_dr_pixel_dy->Integral());
  if (h_dr_impact->Integral() > 0) h_dr_impact->Scale(1.0/h_dr_impact->Integral());
  if (h_dr_outer->Integral() > 0) h_dr_outer->Scale(1.0/h_dr_outer->Integral());
  if (h_dr_outerPixel->Integral() > 0) h_dr_outerPixel->Scale(1.0/h_dr_outerPixel->Integral());
  if (h_dr_closest->Integral() > 0) h_dr_closest->Scale(1.0/h_dr_closest->Integral());

  h_dr_inner->SetLineColor(kBlack);
  h_dr_impact->SetLineColor(kBlue);
  h_dr_outer->SetLineColor(kRed);
  h_dr_outerPixel->SetLineColor(kGreen+2);
  h_dr_closest->SetLineColor(kOrange+1);

  h_dr_inner->SetStats(0);
  h_dr_impact->SetStats(0);
  h_dr_outer->SetStats(0);
  h_dr_outerPixel->SetStats(0);
  h_dr_closest->SetStats(0);
  
  // Find max for Y range
  double maxVal = std::max({h_dr_inner->GetMaximum(), h_dr_impact->GetMaximum(), h_dr_outer->GetMaximum(), h_dr_outerPixel->GetMaximum(), h_dr_closest->GetMaximum()});
  h_dr_inner->SetMaximum(maxVal * 1.1);

  h_dr_inner->SetTitle("dr distribution (Normalized Shapes);dr [cm];Probability Density");
  h_dr_inner->Draw("HIST");
  h_dr_impact->Draw("HIST SAME");
  h_dr_outer->Draw("HIST SAME");
  h_dr_outerPixel->Draw("HIST SAME");
  h_dr_closest->Draw("HIST SAME");

  TLegend *leg = new TLegend(0.4, 0.6, 0.85, 0.88);
  leg->AddEntry(h_dr_inner, "Innermost (shape)", "l");
  leg->AddEntry(h_dr_impact, "Impact Point (shape)", "l");
  leg->AddEntry(h_dr_outer, "Outermost (shape)", "l");
  leg->AddEntry(h_dr_outerPixel, "Outermost Pixel (shape)", "l");
  leg->AddEntry(h_dr_closest, "Closest Hit (shape)", "l");
  leg->Draw();

  c->SaveAs("dr_comparison.png");

  TCanvas *cOut = new TCanvas("cOut", "dr distribution Outermost (raw)", 1000, 800);
  h_dr_outer->SetFillColor(kRed);
  h_dr_outer->SetFillStyle(3001);
  h_dr_outer->SetStats(1);
  h_dr_outer->Draw("HIST");
  cOut->SaveAs("dr_outermost.png");
}

DEFINE_FWK_MODULE(TrackPropAnalyzer);
