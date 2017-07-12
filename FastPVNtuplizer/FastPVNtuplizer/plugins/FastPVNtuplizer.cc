// -*- C++ -*-
//
// Package:    FastPVNtuplizer/FastPVNtuplizer
// Class:      FastPVNtuplizer
// 
/**\class FastPVNtuplizer FastPVNtuplizer.cc FastPVNtuplizer/FastPVNtuplizer/plugins/FastPVNtuplizer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Silvio Donato
//         Created:  Tue, 04 Jul 2017 14:08:25 GMT
//
//


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"


#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/Vertex/interface/SimVertex.h"


#include "TTree.h"
#include "TLorentzVector.h"

#define max_jets 100
#define max_clusters 1000000 
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class FastPVNtuplizer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit FastPVNtuplizer(const edm::ParameterSet&);
      ~FastPVNtuplizer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   int run,event,lumi;
   float zTrue;
    
   int njets;
   float jet_pt[max_jets],jet_eta[max_jets],jet_phi[max_jets],jet_pz[max_jets],jet_mass[max_jets];
   
   int nclusters;
   int cluster_sizeX[max_clusters], cluster_sizeY[max_clusters], cluster_charge[max_clusters],cluster_minDphi_jetMatch[max_clusters];
   float cluster_x[max_clusters],cluster_y[max_clusters],cluster_z[max_clusters],cluster_phi[max_clusters],cluster_minDphi[max_clusters];
   
   TTree* tree;
   edm::Service<TFileService> file; 
   
   
   edm::EDGetTokenT<SiPixelClusterCollectionNew> clustersToken;
   edm::EDGetTokenT<reco::BeamSpot> beamSpotToken;
   edm::EDGetTokenT<edm::View<reco::Jet> > jetsToken;
   edm::EDGetTokenT<std::vector<SimVertex> > simVertexToken;
   
   float dphi;
   
   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FastPVNtuplizer::FastPVNtuplizer(const edm::ParameterSet& iConfig)

{

  tree=file->make<TTree>("tree","tree");
  tree->Branch("event",&event,"event/I");
  tree->Branch("run",&run,"run/I");
  tree->Branch("lumi",&lumi,"lumi/I");

  tree->Branch("zTrue", &zTrue, "zTrue/F");
  tree->Branch("nclusters", &nclusters, "nclusters/I");
  tree->Branch("cluster_sizeX", cluster_sizeX, "cluster_sizeX[nclusters]/I");
  tree->Branch("cluster_sizeY", cluster_sizeY, "cluster_sizeY[nclusters]/I");
  tree->Branch("cluster_charge", cluster_charge, "cluster_charge[nclusters]/I");
  tree->Branch("cluster_x", cluster_x, "cluster_x[nclusters]/F");
  tree->Branch("cluster_y", cluster_y, "cluster_y[nclusters]/F");
  tree->Branch("cluster_z", cluster_z, "cluster_z[nclusters]/F");
  tree->Branch("cluster_phi", cluster_phi, "cluster_phi[nclusters]/F");
  tree->Branch("cluster_minDphi_jetMatch", cluster_minDphi_jetMatch, "cluster_minDphi_jetMatch[nclusters]/I");
  tree->Branch("cluster_minDphi", cluster_minDphi, "cluster_minDphi[nclusters]/F");
  
  tree->Branch("njets", &njets, "njets/I");
  tree->Branch("jet_pt", jet_pt, "jet_pt[njets]/F");
  tree->Branch("jet_eta", jet_eta, "jet_eta[njets]/F");
  tree->Branch("jet_phi", jet_phi, "jet_phi[njets]/F");
  tree->Branch("jet_pz", jet_pz, "jet_pz[njets]/F");
  tree->Branch("jet_mass", jet_mass, "jet_mass[njets]/F");
  
  
   //now do what ever initialization is needed
   usesResource("TFileService");

  clustersToken = consumes<SiPixelClusterCollectionNew>(edm::InputTag("hltSiPixelClusters"));
  beamSpotToken = consumes<reco::BeamSpot>(edm::InputTag("hltOnlineBeamSpot"));
  jetsToken = consumes<edm::View<reco::Jet> >(edm::InputTag("hltAK4CaloJetsCorrected"));
  simVertexToken = consumes<std::vector<SimVertex> >(edm::InputTag("g4SimHits"));


}


//process.hltFastPrimaryVertex = cms.EDProducer("FastPrimaryVertexWithWeightsProducer",
//    EC_weight = cms.double(0.008),
//    PixelCellHeightOverWidth = cms.double(1.8),
//    barrel = cms.bool(True),
//    beamSpot = cms.InputTag("hltOnlineBeamSpot"),
//    clusters = cms.InputTag("hltSiPixelClustersRegForBTag"),
//    endCap = cms.bool(True),
//    jets = cms.InputTag("hltSelector4CentralJetsL1FastJet"),
//    maxDeltaPhi = cms.double(0.21),
//    maxDeltaPhi_EC = cms.double(0.14),
//    maxJetEta = cms.double(2.6),
//    maxJetEta_EC = cms.double(2.6),
//    maxSizeX = cms.double(2.1),
//    maxSizeY_q = cms.double(2.0),
//    maxZ = cms.double(19.0),
//    minJetEta_EC = cms.double(1.3),
//    minJetPt = cms.double(0.0),
//    minSizeY_q = cms.double(-0.6),
//    njets = cms.int32(999),
//    peakSizeY_q = cms.double(1.0),
//    pixelCPE = cms.string('hltESPPixelCPEGeneric'),
//    ptWeighting = cms.bool(True),
//    ptWeighting_offset = cms.double(-1.0),
//    ptWeighting_slope = cms.double(0.05),
//    weightCut_step2 = cms.double(0.05),
//    weightCut_step3 = cms.double(0.1),
//    weight_SizeX1 = cms.double(0.88),
//    weight_charge_down = cms.double(11000.0),
//    weight_charge_peak = cms.double(22000.0),
//    weight_charge_up = cms.double(190000.0),
//    weight_dPhi = cms.double(0.13888888),
//    weight_dPhi_EC = cms.double(0.064516129),
//    weight_rho_up = cms.double(22.0),
//    zClusterSearchArea_step2 = cms.double(3.0),
//    zClusterSearchArea_step3 = cms.double(0.55),
//    zClusterWidth_step1 = cms.double(2.0),
//    zClusterWidth_step2 = cms.double(0.65),
//    zClusterWidth_step3 = cms.double(0.3)
//)


FastPVNtuplizer::~FastPVNtuplizer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
FastPVNtuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;
   using namespace std;

   event = iEvent.eventAuxiliary().event();
   lumi = iEvent.eventAuxiliary().luminosityBlock();
   run = iEvent.eventAuxiliary().run();
   
   //sim vertex
   Handle<vector<SimVertex> > svH;
   iEvent.getByToken(simVertexToken,svH);
   const auto & simVertex = *svH.product();
   zTrue = simVertex.at(0).position().z();
   
   //get pixel cluster
   Handle<SiPixelClusterCollectionNew> cH;
   iEvent.getByToken(clustersToken,cH);
   const SiPixelClusterCollectionNew & pixelClusters = *cH.product();

   //get jets
   Handle<edm::View<reco::Jet> > jH;
   iEvent.getByToken(jetsToken,jH);
//   const edm::View<reco::Jet> & jets = *jH.product();
   
   std::string m_pixelCPE = std::string("hltESPPixelCPEGeneric");
   //get PixelClusterParameterEstimator
   edm::ESHandle<PixelClusterParameterEstimator> pe; 
   const PixelClusterParameterEstimator * pp ;
   iSetup.get<TkPixelCPERecord>().get(m_pixelCPE , pe );  
   pp = pe.product();
   
   //get beamSpot
   edm::Handle<BeamSpot> beamSpot;
   iEvent.getByToken(beamSpotToken,beamSpot);
   
   //get TrackerGeometry
   edm::ESHandle<TrackerGeometry> tracker;
   iSetup.get<TrackerDigiGeometryRecord>().get(tracker);
   const TrackerGeometry * trackerGeometry = tracker.product();

   int jet_count=0;
   nclusters=0;
   njets=0;
   for(auto jit = jH->begin() ; jit != jH->end() ; jit++)
   {//loop on selected jets
     if((jit)->pt()<20) continue;
     if(njets>=max_jets) break;
     jet_pt[njets]=(jit)->pt();
     jet_eta[njets]=(jit)->eta();
     jet_phi[njets]=(jit)->phi();
     jet_pz[njets]=(jit)->pz();
     jet_mass[njets]=(jit)->mass();
     njets++;
     jet_count++;
    }//loop on selected jets
     
     for(SiPixelClusterCollectionNew::const_iterator it = pixelClusters.begin() ; it != pixelClusters.end() ; it++) //Loop on pixel modules with clusters
     {//loop on pixel modules
        DetId id = it->detId();
        const edmNew::DetSet<SiPixelCluster> & detset  = (*it);
        for(size_t j = 0 ; j < detset.size() ; j ++) 
        {//loop on pixel clusters on this module
          const SiPixelCluster & aCluster =  detset[j];
          Point3DBase<float, GlobalTag> v = trackerGeometry->idToDet(id)->surface().toGlobal(pp->localParametersV( aCluster,( *trackerGeometry->idToDetUnit(id)))[0].first) ;
          GlobalPoint v_bs(v.x()-beamSpot->x0(),v.y()-beamSpot->y0(),v.z()-beamSpot->y0());
          
          if(nclusters>=max_clusters) break;
          cluster_minDphi[nclusters] = 999.;
          for(int i=0; i<njets;i++){
            dphi = TVector2::Phi_mpi_pi(jet_phi[i]-v_bs.phi());
            cluster_minDphi[nclusters] = min(abs(dphi),cluster_minDphi[nclusters]);
            cluster_minDphi_jetMatch[nclusters] = i;
          }
          if(cluster_minDphi[nclusters]>0.6) continue;
          cluster_sizeX[nclusters] = aCluster.sizeX();
          cluster_sizeY[nclusters] = aCluster.sizeY();
          cluster_charge[nclusters] = aCluster.charge();
          cluster_x[nclusters] = v_bs.x();
          cluster_y[nclusters] = v_bs.y();
          cluster_z[nclusters] = v_bs.z();
          cluster_phi[nclusters] = v_bs.phi();
          nclusters++;
        }//loop on pixel clusters on this module
    }//loop on pixel modules
tree->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void 
FastPVNtuplizer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
FastPVNtuplizer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
FastPVNtuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(FastPVNtuplizer);
