#include "nn_treeProducer_raw.h"

auto deltaR(float e1, float e2, float p1, float p2) {
	                   
       auto dp = std::abs(p1 -p2);
       if (dp > float(M_PI))
           dp -= float(2 * M_PI);
       return std::sqrt(pow((e1 - e2), 2) + pow(dp, 2));
}    

nn_tupleProducer_raw::nn_tupleProducer_raw(const edm::ParameterSet& conf):
  propagatorToken_(esConsumes(edm::ESInputTag("", "PropagatorWithMaterialParabolicMf")))
  ,ttbToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder")))
  {
  inputTagClusters       = conf.getParameter<edm::InputTag>("siStripClustersTag");
  vertexToken_ = consumes<reco::VertexCollection>(conf.getParameter<edm::InputTag>("vertex"));
  clusterToken           = consumes<edmNew::DetSetVector<SiStripCluster>>(inputTagClusters);
  tracksToken_           = consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("tracks"));
  hlttracksToken_           = consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("hlttracks"));
  hltPixeltracksToken_           = consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("hltPixeltracks"));
  stripCPEToken_         = esConsumes<StripClusterParameterEstimator, TkStripCPERecord>(edm::ESInputTag("", "StripCPEfromTrackAngle"));
  tTopoToken_ = esConsumes<TrackerTopology, TrackerTopologyRcd>();
  tkGeomToken_ = esConsumes();
  usesResource("TFileService");

  beamSpot_ = conf.getParameter<edm::InputTag>("beamSpot");
  beamSpotToken_ = consumes<reco::BeamSpot>(beamSpot_);

  stripNoiseToken_ = esConsumes();

  hist_sig = fs->make<TH2F>("adc_idx_sig","", 50,0,50,260,0,260);
  hist_bkg = fs->make<TH2F>("adc_idx_bkg","", 50,0,50,260,0,260);
  hist_pt_dr = fs->make<TH2F>("pt_dr","", 100,0,2, 50,0,5);
  h_dr = fs->make<TH1F>("dr", "dr", 100,0,1.);
  h_dp = fs->make<TH1F>("dp", "dp", 100,0,20);
  create_tree();
}

nn_tupleProducer_raw::~nn_tupleProducer_raw() = default;

void nn_tupleProducer_raw::analyze(const edm::Event& event, const edm::EventSetup& es) {
  edm::Handle<edmNew::DetSetVector<SiStripCluster>> clusterCollection = event.getHandle(clusterToken);
  const auto& tracksHandle = event.getHandle(tracksToken_);
  const auto& hlttracksHandle = event.getHandle(hlttracksToken_);
  const auto& hltPixeltracksHandle = event.getHandle(hltPixeltracksToken_);
  const auto* stripCPE    = &es.getData(stripCPEToken_);

  const auto& theNoise_ = &es.getData(stripNoiseToken_);

  const Propagator* thePropagator = &es.getData(propagatorToken_);

  theTTrackBuilder = &es.getData(ttbToken_);
  using namespace edm;

  const auto& vertexHandle = event.getHandle(vertexToken_);

  if (!tracksHandle.isValid()) {
    edm::LogError("flatNtuple_producer") << "No valid track collection found";
    return;
  }

  if (!hlttracksHandle.isValid()) {
     edm::LogError("flatNtuple_producer") << "No valid track collection found";
     return;
  }
  if (!hltPixeltracksHandle.isValid()) {
     edm::LogError("flatNtuple_producer") << "No valid track collection found";
     return;
  }
  
  if (!vertexHandle.isValid()) {
    edm::LogError("flatNtuple_producer") << "No valid vertex collection found";
    return;
  }

  const reco::TrackCollection* tracks = tracksHandle.product();
  const reco::TrackCollection* hlttracks = hlttracksHandle.product();
  const reco::TrackCollection* hltPixeltracks = hltPixeltracksHandle.product();
  const reco::VertexCollection& vertices = *vertexHandle;
  //if ( tracks.size() != 1 || event.id().event() !=33) return;
  //std::cout << "event " << event.id().event() << std::endl;
  std::map<uint32_t, std::vector<cluster_property>> matched_cluster;
  //int trkcluster = 0;
  for(unsigned int i=0; i<tracks->size(); i++) {
     auto& trk = tracks->at(i);
     for (auto ih = trk.recHitsBegin(); ih != trk.recHitsEnd(); ih++) {
         const SiStripCluster* strip=NULL;
         const TrackingRecHit& hit = **ih;
         const DetId detId((hit).geographicalId());
         if (detId.det() == DetId::Tracker) { 
           if (detId.subdetId() == kBPIX || detId.subdetId() == kFPIX) continue;  // pixel is always 2D
           else {        // should be SiStrip now
		   //std::cout << "subdet " << detId.subdetId() << std::endl;
               if (dynamic_cast<const SiStripRecHit1D *>(&hit)) {
		   //std::cout << " found SiStripRecHit1D " << std::endl;
                   strip = dynamic_cast<const SiStripRecHit1D *>(&hit)->cluster().get();
               }
               else if ( dynamic_cast<const SiStripRecHit2D *>(&hit)) {
                 //std::cout << "found SiStripRecHit2D " << std::endl;
                 strip = dynamic_cast<const SiStripRecHit2D *>(&hit)->cluster().get();
               }
               else if (dynamic_cast<const SiStripMatchedRecHit2D *>(&hit)) {
                  //std::cout << "found SiStripMatchedRecHit2D " << std::endl;
                  strip = &(dynamic_cast<const SiStripMatchedRecHit2D *>(&hit))->monoCluster();
              }
           }
           if(strip) {
		   //trkcluster += 1;
	       //std::cout << "strip " << strip << std::endl;
               bool low_pt_trk = trk.pt() < 1.;
               matched_cluster[detId].emplace_back(
                      low_pt_trk, !low_pt_trk, strip->barycenter(),
                      strip->size(), strip->firstStrip(), strip->endStrip(),
                      strip->charge(),
                      trk.algo()
               );
         }
        }
    }
  }    
  const auto& tkGeom = &es.getData(tkGeomToken_);
  const auto tkDets = tkGeom->dets();
  const TrackerTopology& tTopo = es.getData(tTopoToken_);
  //int cluster = 0;
  for (const auto& detSiStripClusters : *clusterCollection) {
    //std::cout << "new detId " << std::endl;
    isTIB = isTOB = isTID = isTEC = isStereo = 0;
    eventN = event.id().event();
    //if (eventN != 24061779) continue;
    runN   = (int) event.id().run();
    lumi   = (int) event.id().luminosityBlock();
    uint32_t detId  = detSiStripClusters.id();
    SiStripNoises::Range detNoiseRange = theNoise_->getRange(detId);
    uint32_t subdet = DetId(detId).subdetId();
    if (subdet == SiStripSubdetector::TIB) isTIB = 1;
    else if (subdet == SiStripSubdetector::TOB) isTOB = 1;
    else if (subdet == SiStripSubdetector::TID) isTID = 1;
    else if (subdet == SiStripSubdetector::TEC) isTEC = 1;
    layer = tTopo.layer(DetId(detId));
    isStereo = tTopo.isStereo(DetId(detId));
    isglued = tTopo.glued(DetId(detId));
    isstacked = tTopo.stack(DetId(detId));
    const auto& _detId = detId; // for the capture clause in the lambda function
    auto det = std::find_if(tkDets.begin(), tkDets.end(), [_detId](auto& elem) -> bool {
        return (elem->geographicalId().rawId() == _detId);
    });
    const StripTopology& p = dynamic_cast<const StripGeomDetUnit*>(*det)->specificTopology();
    std::vector<cluster_property> track_clusters = {};
    if ( matched_cluster.find(detId) != matched_cluster.end() ) track_clusters = matched_cluster[detId];

    std::map<reco::TrackRef, TrajectoryStateOnSurface> recotrk_tsosCache;
    std::map<reco::TrackRef, TrajectoryStateOnSurface> hlttrk_tsosCache;
    std::map<reco::TrackRef, TrajectoryStateOnSurface> pixeltrk_tsosCache;

    const GeomDetUnit* geomDet = tkGeom->idToDetUnit(detId);
    const StripGeomDetUnit* stripDet = dynamic_cast<const StripGeomDetUnit*>(geomDet);

    auto tkgeom_surf = (tkGeom->idToDet(detId))->surface();

    bool firstcluster = true;

    for (const auto& stripCluster : detSiStripClusters) {
    //  cluster += 1;
      //std::cout << "new cluster " << std::endl;
      initialize_vars();
      uint16_t firstStrip = stripCluster.firstStrip();
      uint16_t endStrip   = stripCluster.endStrip();
      float barycenter = stripCluster.barycenter();
      size       = stripCluster.size();
      charge     = stripCluster.charge();

      std::vector<int>adcs;
      std::vector<float>noises;
     
      float hitX[nMax], hitY[nMax], hitZ[nMax]; 
      for (int strip = firstStrip; strip < endStrip; ++strip)
      {
        GlobalPoint gp = tkgeom_surf.toGlobal(p.localPosition((float) strip));

        hitX   [strip - firstStrip] = gp.x();
        hitY   [strip - firstStrip] = gp.y();
        hitZ   [strip - firstStrip] = gp.z();
        adc    [strip - firstStrip] = stripCluster[strip - firstStrip];
	adcs.push_back(stripCluster[strip - firstStrip]);
	noises.push_back(theNoise_->getNoise(strip, detNoiseRange));
	if (adc    [strip - firstStrip] >= 254) n_saturated += 1;
      }
      for(size_t i=0; i<adcs.size(); i++) {
	 if(i<10) {
	    adcs_ten[i] = adcs[i];
	    //noises_ten[i] = noises[i];
	 }
      }
      
      mean_x = std::accumulate(hitX, hitX+size, 0.0) / size;
      mean_y = std::accumulate(hitY, hitY+size, 0.0) / size; 
      mean_z = std::accumulate(hitZ, hitZ+size, 0.0) / size;

      for(auto& trk_cluster_property: track_clusters)
        {
           if (trk_cluster_property.barycenter == barycenter)
           {
               assert( (size == trk_cluster_property.size)
                      && (firstStrip == trk_cluster_property.firstStrip)
                      && (endStrip == trk_cluster_property.endStrip)
                      && (charge == trk_cluster_property.charge)
               );
               low_pt_trk_cluster = trk_cluster_property.low_pt_trk_cluster;
               high_pt_trk_cluster = trk_cluster_property.high_pt_trk_cluster;
               trk_algo           = trk_cluster_property.trk_algo;
	       target = 1;
	       break;
           }
        }

      const reco::Track* recotrk = NULL;
      const reco::Track* pixeltrk = NULL;
      const reco::Track* hlttrk = NULL;
      hlttrk_dr_min = 99;
      recotrk_dr_min = 99;
      pixeltrk_dr_min = 99.;

     if ( firstcluster ) {
      for ( size_t i=0; i<tracks->size(); i++) {
        reco::TrackRef trackRef(tracks, i);     
        TrajectoryStateOnSurface tsos;
	for (auto const& hit : trackRef->recHits()) {
             if (!hit->isValid()) continue;
             if (hit->geographicalId() != detId) continue;
             reco::TransientTrack tkTT = theTTrackBuilder->build(*trackRef);
             tsos = thePropagator->propagate(tkTT.innermostMeasurementState(), geomDet->surface());
             if ( tsos.isValid() ) recotrk_tsosCache[trackRef] = tsos;
             break;
        }
      }
     }

     for (const auto& [trackRef, tsos] : recotrk_tsosCache) {
       auto localValues = stripCPE->localParameters(stripCluster, *stripDet, tsos);
       LocalPoint clusterLocal = localValues.first;
       LocalPoint trackLocal = geomDet->surface().toLocal(tsos.globalPosition());
       float dr = abs(trackLocal.x() - clusterLocal.x()); //std::sqrt( pow( (trackLocal.x() - clusterLocal.x()), 2 ) +
       if (dr < recotrk_dr_min) {
           recotrk_dr_min = dr;
           recotrk = &(*trackRef);
       }
     }
     if (recotrk) {
       recotrk_pt = recotrk->pt();
       recotrk_pterr = recotrk->ptError();
       recotrk_eta = recotrk->eta();
       recotrk_phi = recotrk->phi();
       recotrk_dz = recotrk->dz(vertices.at(0).position());
       recotrk_dxy = recotrk->dxy(vertices.at(0).position());
       recotrk_validhits = recotrk->numberOfValidHits();
       recotrk_validpixelhits = recotrk->hitPattern().numberOfValidPixelHits();
       recotrk_chi2 = recotrk->normalizedChi2();
       recotrk_d0sigma = sqrt(recotrk->d0Error() * recotrk->d0Error() + vertices.at(0).xError() * vertices.at(0).yError());
       recotrk_dzsigma = sqrt(recotrk->dzError() * recotrk->dzError() + vertices.at(0).zError() * vertices.at(0).zError());
       recotrk_qoverp = recotrk->qoverp();
       recotrk_qoverperror = recotrk->qoverpError();
     }
      
      //regression->Fill();
      //continue;

     if ( firstcluster ) {
       for ( size_t i=0; i<hltPixeltracks->size(); i++) {
          reco::TrackRef trackRef(hltPixeltracks, i);
          TrajectoryStateOnSurface tsos;
          reco::TransientTrack tkTT = theTTrackBuilder->build(*trackRef);
          if(!tkTT.impactPointState().isValid()) continue;
          tsos = thePropagator->propagate(tkTT.impactPointState(), geomDet->surface());
          if( tsos.isValid() ) pixeltrk_tsosCache[trackRef] = tsos;
       }
     }
      //std::cout << "reco trk size: " << recotrk_tsosCache.size() << "\trecotrk valid pixelhits: " << recotrk_validpixelhits << "\tpixeltrk size: " << pixeltrk_tsosCache.size() << std::endl;

      int good_sharedhits = 0;
      float dr_reco_vs_pixel = 99;
      float dp_reco_vs_pixel = 99;
      if ( recotrk_validpixelhits ) { //&& recotrk_pt >= 0.6) {
        for (const auto& [trackRef, tsos] : pixeltrk_tsosCache) {
          int sharedhits(0);
          for (auto const& pixelhit : trackRef->recHits()) {
            if ( !pixelhit->isValid() ) continue;
            for (auto const& rechit : recotrk->recHits()) {
               if ( !rechit->isValid() ) continue;
	       if (pixelhit->geographicalId() != rechit->geographicalId()()) continue;
               sharedhits += 1;
               break;
            }
	  }
          if ( (sharedhits >= (recotrk_validpixelhits*0.5)) && sharedhits >= good_sharedhits ) {
             //std::cout << "sharedhits: " << sharedhits << "\tgood_sharedhits: " << good_sharedhits << std::endl;
	     if ( good_sharedhits == sharedhits ) {
		float dr = deltaR(recotrk->eta(), trackRef->eta(), recotrk->phi(), trackRef->phi());
		float dp = abs(recotrk->pt() - trackRef->pt()) / recotrk->pt();
		if (!(dr<dr_reco_vs_pixel) || !(dp<dp_reco_vs_pixel)) continue;
	     }
             auto localValues = stripCPE->localParameters(stripCluster, *stripDet, tsos);
	     LocalPoint clusterLocal = localValues.first;
	     LocalPoint trackLocal = geomDet->surface().toLocal(tsos.globalPosition());
	     float dr = abs(trackLocal.x() - clusterLocal.x());
             good_sharedhits = sharedhits;
             pixeltrk_dr_min = dr;
             pixeltrk = &(*trackRef);
	     dr_reco_vs_pixel = deltaR(recotrk->eta(), pixeltrk->eta(), recotrk->phi(), pixeltrk->phi());
	     dp_reco_vs_pixel = abs(recotrk->pt() - pixeltrk->pt()) / recotrk->pt();
             //std::cout << "pixetrk chi2: " << trackRef->normalizedChi2() << "\tpixeltrk eta: " << abs(trackRef->eta()) << "\tpixeltrk valid hits: " << trackRef->numberOfValidHits() << "\trecotrk validhits: " << recotrk_validpixelhits << "\tpixeltrk_drmin: " << pixeltrk_dr_min << "\trecotrk_drmin:" << recotrk_dr_min << std::endl; 
	  }
	}
      }

      if (pixeltrk) {
        pixeltrk_pt = pixeltrk->pt();
        pixeltrk_pterr = pixeltrk->ptError();
        pixeltrk_eta = pixeltrk->eta();
        pixeltrk_phi = pixeltrk->phi();
        pixeltrk_dz = pixeltrk->dz(vertices.at(0).position());
        pixeltrk_dxy = pixeltrk->dxy(vertices.at(0).position());
        pixeltrk_validhits = pixeltrk->numberOfValidHits();
	pixeltrk_validsharedhits = good_sharedhits;
        assert(pixeltrk->hitPattern().numberOfValidPixelHits() == pixeltrk->numberOfValidHits());
        pixeltrk_chi2 = pixeltrk->normalizedChi2();
        pixeltrk_d0sigma = sqrt(pixeltrk->d0Error() * pixeltrk->d0Error() + vertices.at(0).xError() * vertices.at(0).yError());
        pixeltrk_dzsigma = sqrt(pixeltrk->dzError() * pixeltrk->dzError() + vertices.at(0).zError() * vertices.at(0).zError());
        pixeltrk_qoverp = pixeltrk->qoverp();
        pixeltrk_qoverperror = pixeltrk->qoverpError();
      }
      /*if (recotrk) {// && pixeltrk) {
	      std::cout << "target: " << target << "\tx:" << mean_x << "y:" << mean_y << "z:" << mean_z << "\tsize:" << size << "\tcharge:" << charge << std::endl;
	      std::cout << "reco,pt:" << recotrk_pt << "\t" << "eta:" << recotrk_eta << "phi:" << recotrk_phi << "\t" << "drmin:" << recotrk_dr_min << std::endl;
              //std::cout << "pixel,pt:" << pixeltrk_pt << "\t" << "eta:" << pixeltrk_eta << "phi:" << pixeltrk_phi << "\t" << "drmin:" << pixeltrk_dr_min << std::endl;
      fillWithOverFlow(hist_pt_dr, recotrk_pt, pixeltrk_dr_min);
      fillWithOverFlow(h_dr, dr_reco_vs_pixel);
      fillWithOverFlow(h_dp,dp_reco_vs_pixel);
      }*/
      if ( firstcluster ) {
         for ( size_t i=0; i<hlttracks->size(); i++) {
          reco::TrackRef trackRef(hlttracks, i);
          TrajectoryStateOnSurface tsos;
	  for (auto const& hit : trackRef->recHits()) {
               if (!hit->isValid()) continue;
               if (hit->geographicalId() != detId) continue;
                reco::TransientTrack tkTT = theTTrackBuilder->build(*trackRef);
                tsos = thePropagator->propagate(tkTT.innermostMeasurementState(), geomDet->surface());
                if ( tsos.isValid() ) hlttrk_tsosCache[trackRef] = tsos;
                break;
             }
          }
      }

      for(const auto& [trackRef, tsos] : hlttrk_tsosCache) {
       auto localValues = stripCPE->localParameters(stripCluster, *stripDet, tsos);
          LocalPoint clusterLocal = localValues.first;

          LocalPoint trackLocal = geomDet->surface().toLocal(tsos.globalPosition());
          float dr = abs(trackLocal.x() - clusterLocal.x()); //std::sqrt( pow( (trackLocal.x() - clusterLocal.x()), 2 ) +
          if (dr < hlttrk_dr_min) {
            hlttrk_dr_min = dr;
            hlttrk = &(*trackRef);
          }
      }
      if (hlttrk) {
        hlttrk_pt = hlttrk->pt();
        hlttrk_pterr = hlttrk->ptError();
        hlttrk_eta = hlttrk->eta();
        hlttrk_phi = hlttrk->phi();
        hlttrk_dz = hlttrk->dz(vertices.at(0).position());
        hlttrk_dxy = hlttrk->dxy(vertices.at(0).position());
        hlttrk_validhits = hlttrk->numberOfValidHits();
	hlttrk_validpixelhits = hlttrk->hitPattern().numberOfValidPixelHits();
        hlttrk_chi2 = hlttrk->normalizedChi2();
        hlttrk_d0sigma = sqrt(hlttrk->d0Error() * hlttrk->d0Error() + vertices.at(0).xError() * vertices.at(0).yError());
        hlttrk_dzsigma = sqrt(hlttrk->dzError() * hlttrk->dzError() + vertices.at(0).zError() * vertices.at(0).zError());
        hlttrk_qoverp = hlttrk->qoverp();
        hlttrk_qoverperror = hlttrk->qoverpError();
      }
      firstcluster = false;
      tree->Fill();
      if (target) {
	      for (unsigned int i = 0; i < adcs.size()-1; ++i) hist_sig->Fill(i, adcs[i]); // Fill 2D histogram
      }
      else {
	      for (unsigned int i = 0; i < adcs.size()-1; ++i) hist_bkg->Fill(i, adcs[i]); // Fill 2D histogram
      }
    }
  }
}

void nn_tupleProducer_raw::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("siStripClustersTag", edm::InputTag("siStripClusters"));
  desc.add<edm::InputTag>("tracks", edm::InputTag("generalTracks","","reRECO"));
  desc.add<edm::InputTag>("hlttracks", edm::InputTag("hltTracks","","HLTX"));
  desc.add<edm::InputTag>("hltPixeltracks", edm::InputTag("hltPixelTracks","","HLTX"));
  desc.add<edm::InputTag>("beamSpot", edm::InputTag("offlineBeamSpot"));
  desc.add<edm::InputTag>("vertex", edm::InputTag("vertex"));
  descriptions.add("nn_tupleProducer_raw", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(nn_tupleProducer_raw);
