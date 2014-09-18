// -*- C++ -*-
//
// Package:    NtuplerVBF
// Class:      NtuplerVBF
// 
/**\class NtuplerVBF NtuplerVBF.cc RecoBTag/NtuplerVBF/src/NtuplerVBF.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrea RIZZI
//         Created:  Thu Dec 22 14:51:44 CET 2011
// $Id: NtuplerVBF.cc,v 1.3 2012/02/02 09:04:04 arizzi Exp $
//
//


// system include files
#include <memory>
#include <vector>
// user include files
 #include "TRef.h"
// #include "TCollection.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/TrackJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/BTauReco/interface/JetTag.h"

#include "CommonTools/Clustering1D/interface/Clusterizer1DCommons.h"
#include "CommonTools/Clustering1D/interface/Cluster1DMerger.h"
#include "CommonTools/Clustering1D/interface/TrivialWeightEstimator.h"
#define HaveMtv
#define HaveFsmw
#define HaveDivisive
#ifdef HaveMtv
#include "CommonTools/Clustering1D/interface/MtvClusterizer1D.h"
#endif
#ifdef HaveFsmw
#include "CommonTools/Clustering1D/interface/FsmwClusterizer1D.h"
#endif
#ifdef HaveDivisive
#include "CommonTools/Clustering1D/interface/DivisiveClusterizer1D.h"
#endif

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"

#include "DataFormats/L1Trigger/interface/L1HFRings.h"

#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"

//************************************************************************
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#define N_jet       100  
#define N_jetL1       10  
#define N_muon      10  
#define N_jetGen       1000  
//************************************************************************

typedef std::map<double, int> mjj_map;

using namespace std;

//
// class declaration
//

//float DeltaPhi(float phi1,float phi2)
//{
//	float deltaPhi = TMath::Abs(phi1-phi2);
//	if(deltaPhi > TMath::Pi())
//	deltaPhi = TMath::TwoPi() - deltaPhi;
//	return deltaPhi;
//}

class NtuplerVBF : public edm::EDProducer {
   public:
      explicit NtuplerVBF(const edm::ParameterSet&);


int nevent,nrun,nlumi;


      int ncjets;
      float cjetPt[N_jetL1],cjetEta[N_jetL1],cjetPhi[N_jetL1];

      int nfwjets;
      float fwjetPt[N_jetL1],fwjetEta[N_jetL1],fwjetPhi[N_jetL1];

      int ntaujets;
      float taujetPt[N_jetL1],taujetEta[N_jetL1],taujetPhi[N_jetL1];

      int njets;
      float jetPt[2*N_jet],jetEta[2*N_jet],jetPhi[2*N_jet];

      int nmuons;
      float muonPt[2*N_muon],muonEta[2*N_muon],muonPhi[2*N_muon];

      float metPt,etTot,etMiss,metEta,metPhi;

      float mhtPt,htTot,htMiss,mhtEta,mhtPhi;

      float metPtGen,metEtaGen,metPhiGen;

      int pu;
	
      float hfring1, hfring2, hfring3, hfring4, hfring0;
      
      
float caloht, calomht, calomhtPhi, caloNoPUht, caloNoPUmht, caloNoPUmhtPhi, pfht, pfmht, pfmhtPhi; 
float caloet, calomet, calometPhi, pfet, pfmet, pfmetPhi; 
float caloetCleanUsingJetID, calometCleanUsingJetID, calometPhiCleanUsingJetID; 
float caloetClean, calometClean, calometPhiClean; 

int ngenjets;
float genjetPt[N_jet],genjetEta[N_jet],genjetPhi[N_jet];

int ncalojets;
float calojetPt[N_jet],calojetEta[N_jet],calojetPhi[N_jet];

int ncaloNoPUjets;
float caloNoPUjetPt[N_jet],caloNoPUjetEta[N_jet],caloNoPUjetPhi[N_jet];

int npfjets;
float pfjetPt[N_jet],pfjetEta[N_jet],pfjetPhi[N_jet];

int ntrackjets;
float trackjetPt[N_jet],trackjetEta[N_jet],trackjetPhi[N_jet];

int nCSVL25;
float CSVL25[N_jet], CSVL25_jetPt[N_jet],CSVL25_jetEta[N_jet],CSVL25_jetPhi[N_jet];

int nCSVL3;
float CSVL3[N_jet], CSVL3_jetPt[N_jet],CSVL3_jetEta[N_jet],CSVL3_jetPhi[N_jet];

int nCSVPF;
float CSVPF[N_jet], CSVPF_jetPt[N_jet],CSVPF_jetEta[N_jet],CSVPF_jetPhi[N_jet];

float b1Eta_gen,b1Phi_gen,b1Pt_gen,b2Eta_gen,b2Phi_gen,b2Pt_gen,q1Eta_gen,q1Phi_gen,q1Pt_gen,q2Eta_gen,q2Phi_gen,q2Pt_gen;

float mqq_etaOrd,deltaetaqq_etaOrd,deltaphibb_etaOrd,deltaetabb_etaOrd,ptsqq_etaOrd,ptsbb_etaOrd,signeta_etaOrd,q1Eta_etaOrd,q1Phi_etaOrd,q1Pt_etaOrd,q2Eta_etaOrd,q2Phi_etaOrd,q2Pt_etaOrd,b1Eta_etaOrd,b1Phi_etaOrd,b1Pt_etaOrd,b2Eta_etaOrd,b2Phi_etaOrd,b2Pt_etaOrd;
float b1deltaR_etaOrd,b2deltaR_etaOrd,q1deltaR_etaOrd,q2deltaR_etaOrd;

float mqq_etaOrdNoPU,deltaetaqq_etaOrdNoPU,deltaphibb_etaOrdNoPU,deltaetabb_etaOrdNoPU,ptsqq_etaOrdNoPU,ptsbb_etaOrdNoPU,signeta_etaOrdNoPU,q1Eta_etaOrdNoPU,q1Phi_etaOrdNoPU,q1Pt_etaOrdNoPU,q2Eta_etaOrdNoPU,q2Phi_etaOrdNoPU,q2Pt_etaOrdNoPU,b1Eta_etaOrdNoPU,b1Phi_etaOrdNoPU,b1Pt_etaOrdNoPU,b2Eta_etaOrdNoPU,b2Phi_etaOrdNoPU,b2Pt_etaOrdNoPU;
float b1deltaR_etaOrdNoPU,b2deltaR_etaOrdNoPU,q1deltaR_etaOrdNoPU,q2deltaR_etaOrdNoPU;

float mqq_etaPFOrd,deltaetaqq_etaPFOrd,deltaphibb_etaPFOrd,deltaetabb_etaPFOrd,ptsqq_etaPFOrd,ptsbb_etaPFOrd,signeta_etaPFOrd,q1Eta_etaPFOrd,q1Phi_etaPFOrd,q1Pt_etaPFOrd,q2Eta_etaPFOrd,q2Phi_etaPFOrd,q2Pt_etaPFOrd,b1Eta_etaPFOrd,b1Phi_etaPFOrd,b1Pt_etaPFOrd,b2Eta_etaPFOrd,b2Phi_etaPFOrd,b2Pt_etaPFOrd;
float b1deltaR_etaPFOrd,b2deltaR_etaPFOrd,q1deltaR_etaPFOrd,q2deltaR_etaPFOrd;

float mqq_etaTrackOrd,deltaetaqq_etaTrackOrd,deltaphibb_etaTrackOrd,deltaetabb_etaTrackOrd,ptsqq_etaTrackOrd,ptsbb_etaTrackOrd,signeta_etaTrackOrd,q1Eta_etaTrackOrd,q1Phi_etaTrackOrd,q1Pt_etaTrackOrd,q2Eta_etaTrackOrd,q2Phi_etaTrackOrd,q2Pt_etaTrackOrd,b1Eta_etaTrackOrd,b1Phi_etaTrackOrd,b1Pt_etaTrackOrd,b2Eta_etaTrackOrd,b2Phi_etaTrackOrd,b2Pt_etaTrackOrd;
float b1deltaR_etaTrackOrd,b2deltaR_etaTrackOrd,q1deltaR_etaTrackOrd,q2deltaR_etaTrackOrd;

float mqq_CSVL25Ord,deltaetaqq_CSVL25Ord,deltaphibb_CSVL25Ord,deltaetabb_CSVL25Ord,ptsqq_CSVL25Ord,ptsbb_CSVL25Ord,signeta_CSVL25Ord,q1Eta_CSVL25Ord,q1Phi_CSVL25Ord,q1Pt_CSVL25Ord,q2Eta_CSVL25Ord,q2Phi_CSVL25Ord,q2Pt_CSVL25Ord,b1Eta_CSVL25Ord,b1Phi_CSVL25Ord,b1Pt_CSVL25Ord,b2Eta_CSVL25Ord,b2Phi_CSVL25Ord,b2Pt_CSVL25Ord;
float b1deltaR_CSVL25Ord,b2deltaR_CSVL25Ord,q1deltaR_CSVL25Ord,q2deltaR_CSVL25Ord;

float mqq_CSVL3Ord,deltaetaqq_CSVL3Ord,deltaphibb_CSVL3Ord,deltaetabb_CSVL3Ord,ptsqq_CSVL3Ord,ptsbb_CSVL3Ord,signeta_CSVL3Ord,q1Eta_CSVL3Ord,q1Phi_CSVL3Ord,q1Pt_CSVL3Ord,q2Eta_CSVL3Ord,q2Phi_CSVL3Ord,q2Pt_CSVL3Ord,b1Eta_CSVL3Ord,b1Phi_CSVL3Ord,b1Pt_CSVL3Ord,b2Eta_CSVL3Ord,b2Phi_CSVL3Ord,b2Pt_CSVL3Ord;
float b1deltaR_CSVL3Ord,b2deltaR_CSVL3Ord,q1deltaR_CSVL3Ord,q2deltaR_CSVL3Ord;

float mqq_CSVPFOrd,deltaetaqq_CSVPFOrd,deltaphibb_CSVPFOrd,deltaetabb_CSVPFOrd,ptsqq_CSVPFOrd,ptsbb_CSVPFOrd,signeta_CSVPFOrd,q1Eta_CSVPFOrd,q1Phi_CSVPFOrd,q1Pt_CSVPFOrd,q2Eta_CSVPFOrd,q2Phi_CSVPFOrd,q2Pt_CSVPFOrd,b1Eta_CSVPFOrd,b1Phi_CSVPFOrd,b1Pt_CSVPFOrd,b2Eta_CSVPFOrd,b2Phi_CSVPFOrd,b2Pt_CSVPFOrd;
float b1deltaR_CSVPFOrd,b2deltaR_CSVPFOrd,q1deltaR_CSVPFOrd,q2deltaR_CSVPFOrd;

  typedef std::pair<double,unsigned int> Jpair;
  static bool comparator ( const Jpair& l, const Jpair& r) {
    return l.first < r.first;
  }
  static bool comparatorInv ( const Jpair& l, const Jpair& r) {
    return l.first > r.first;
  }
   private:
      virtual void produce(edm::Event&, const edm::EventSetup&);
	TTree *tree;
  	edm::Service<TFileService> file;	

float dRj4_4b[3]; float Mjj_diffOrdered_4b[6]; float Mjj_4b[6];

  //************************************
     
};

NtuplerVBF::NtuplerVBF(const edm::ParameterSet& iConfig)
{
	tree=file->make<TTree>("tree","tree");
	tree->Branch("nevent",&nevent,"nevent/I");
	tree->Branch("nrun",&nrun,"nrun/I");
	tree->Branch("nlumi",&nlumi,"nlumi/I");

	tree->Branch("ngenjets",&ngenjets,"ngenjets/I");
	tree->Branch("genjetPt",genjetPt,"genjetPt[ngenjets]/F");
	tree->Branch("genjetEta",genjetEta,"genjetEta[ngenjets]/F");
	tree->Branch("genjetPhi",genjetPhi,"genjetPhi[ngenjets]/F");

	tree->Branch("ncalojets",&ncalojets,"ncalojets/I");
	tree->Branch("calojetPt",calojetPt,"calojetPt[ncalojets]/F");
	tree->Branch("calojetEta",calojetEta,"calojetEta[ncalojets]/F");
	tree->Branch("calojetPhi",calojetPhi,"calojetPhi[ncalojets]/F");

	tree->Branch("npfjets",&npfjets,"npfjets/I");
	tree->Branch("pfjetPt",pfjetPt,"pfjetPt[npfjets]/F");
	tree->Branch("pfjetEta",pfjetEta,"pfjetEta[npfjets]/F");
	tree->Branch("pfjetPhi",pfjetPhi,"pfjetPhi[npfjets]/F");

	tree->Branch("ntrackjets",&ntrackjets,"ntrackjets/I");
	tree->Branch("trackjetPt",trackjetPt,"trackjetPt[ntrackjets]/F");
	tree->Branch("trackjetEta",trackjetEta,"trackjetEta[ntrackjets]/F");
	tree->Branch("trackjetPhi",trackjetPhi,"trackjetPhi[ntrackjets]/F");

	tree->Branch("ncaloNoPUjets",&ncaloNoPUjets,"ncaloNoPUjets/I");
	tree->Branch("caloNoPUjetPt",caloNoPUjetPt,"caloNoPUjetPt[ncaloNoPUjets]/F");
	tree->Branch("caloNoPUjetEta",caloNoPUjetEta,"caloNoPUjetEta[ncaloNoPUjets]/F");
	tree->Branch("caloNoPUjetPhi",caloNoPUjetPhi,"caloNoPUjetPhi[ncaloNoPUjets]/F");

	tree->Branch("nCSVL25",&nCSVL25,"nCSVL25/I");
	tree->Branch("CSVL25",CSVL25,"CSVL25[nCSVL25]/F");
	tree->Branch("CSVL25_jetPt",CSVL25_jetPt,"CSVL25_jetPt[nCSVL25]/F");
	tree->Branch("CSVL25_jetEta",CSVL25_jetEta,"CSVL25_jetEta[nCSVL25]/F");
	tree->Branch("CSVL25_jetPhi",CSVL25_jetPhi,"CSVL25_jetPhi[nCSVL25]/F");

	tree->Branch("nCSVL3",&nCSVL3,"nCSVL3/I");
	tree->Branch("CSVL3",CSVL3,"CSVL3[nCSVL3]/F");
	tree->Branch("CSVL3_jetPt",CSVL3_jetPt,"CSVL3_jetPt[nCSVL3]/F");
	tree->Branch("CSVL3_jetEta",CSVL3_jetEta,"CSVL3_jetEta[nCSVL3]/F");
	tree->Branch("CSVL3_jetPhi",CSVL3_jetPhi,"CSVL3_jetPhi[nCSVL3]/F");

	tree->Branch("nCSVPF",&nCSVPF,"nCSVPF/I");
	tree->Branch("CSVPF",CSVPF,"CSVPF[nCSVPF]/F");
	tree->Branch("CSVPF_jetPt",CSVPF_jetPt,"CSVPF_jetPt[nCSVPF]/F");
	tree->Branch("CSVPF_jetEta",CSVPF_jetEta,"CSVPF_jetEta[nCSVPF]/F");
	tree->Branch("CSVPF_jetPhi",CSVPF_jetPhi,"CSVPF_jetPhi[nCSVPF]/F");

	tree->Branch("b1Eta_gen",&b1Eta_gen,"b1Eta_gen/F");
	tree->Branch("b1Phi_gen",&b1Phi_gen,"b1Phi_gen/F");
	tree->Branch("b1Pt_gen",&b1Pt_gen,"b1Pt_gen/F");

	tree->Branch("b2Eta_gen",&b2Eta_gen,"b2Eta_gen/F");
	tree->Branch("b2Phi_gen",&b2Phi_gen,"b2Phi_gen/F");
	tree->Branch("b2Pt_gen",&b2Pt_gen,"b2Pt_gen/F");

	tree->Branch("q1Eta_gen",&q1Eta_gen,"q1Eta_gen/F");
	tree->Branch("q1Phi_gen",&q1Phi_gen,"q1Phi_gen/F");
	tree->Branch("q1Pt_gen",&q1Pt_gen,"q1Pt_gen/F");

	tree->Branch("q2Eta_gen",&q2Eta_gen,"q2Eta_gen/F");
	tree->Branch("q2Phi_gen",&q2Phi_gen,"q2Phi_gen/F");
	tree->Branch("q2Pt_gen",&q2Pt_gen,"q2Pt_gen/F");

	tree->Branch("mqq_etaOrd",&mqq_etaOrd,"mqq_etaOrd/F");
	tree->Branch("deltaetaqq_etaOrd",&deltaetaqq_etaOrd,"deltaetaqq_etaOrd/F");
	tree->Branch("deltaetabb_etaOrd",&deltaetabb_etaOrd,"deltaetabb_etaOrd/F");
	tree->Branch("deltaphibb_etaOrd",&deltaphibb_etaOrd,"deltaphibb_etaOrd/F");
	tree->Branch("ptsqq_etaOrd",&ptsqq_etaOrd,"ptsqq_etaOrd/F");
	tree->Branch("ptsbb_etaOrd",&ptsbb_etaOrd,"ptsbb_etaOrd/F");
	tree->Branch("signeta_etaOrd",&signeta_etaOrd,"signeta_etaOrd/F");
	tree->Branch("q1Eta_etaOrd",&q1Eta_etaOrd,"q1Eta_etaOrd/F");
	tree->Branch("q1Phi_etaOrd",&q1Phi_etaOrd,"q1Phi_etaOrd/F");
	tree->Branch("q1Pt_etaOrd",&q1Pt_etaOrd,"q1Pt_etaOrd/F");
	tree->Branch("q2Eta_etaOrd",&q2Eta_etaOrd,"q2Eta_etaOrd/F");
	tree->Branch("q2Phi_etaOrd",&q2Phi_etaOrd,"q2Phi_etaOrd/F");
	tree->Branch("q2Pt_etaOrd",&q2Pt_etaOrd,"q2Pt_etaOrd/F");
	tree->Branch("b1Eta_etaOrd",&b1Eta_etaOrd,"b1Eta_etaOrd/F");
	tree->Branch("b1Phi_etaOrd",&b1Phi_etaOrd,"b1Phi_etaOrd/F");
	tree->Branch("b1Pt_etaOrd",&b1Pt_etaOrd,"b1Pt_etaOrd/F");
	tree->Branch("b2Eta_etaOrd",&b2Eta_etaOrd,"b2Eta_etaOrd/F");
	tree->Branch("b2Phi_etaOrd",&b2Phi_etaOrd,"b2Phi_etaOrd/F");
	tree->Branch("b2Pt_etaOrd",&b2Pt_etaOrd,"b2Pt_etaOrd/F");
	
	tree->Branch("q1deltaR_etaOrd",&q1deltaR_etaOrd,"q1deltaR_etaOrd/F");	
	tree->Branch("q2deltaR_etaOrd",&q2deltaR_etaOrd,"q2deltaR_etaOrd/F");	
	tree->Branch("b1deltaR_etaOrd",&b1deltaR_etaOrd,"b1deltaR_etaOrd/F");	
	tree->Branch("b2deltaR_etaOrd",&b2deltaR_etaOrd,"b2deltaR_etaOrd/F");	

	tree->Branch("mqq_etaOrdNoPU",&mqq_etaOrdNoPU,"mqq_etaOrdNoPU/F");
	tree->Branch("deltaetaqq_etaOrdNoPU",&deltaetaqq_etaOrdNoPU,"deltaetaqq_etaOrdNoPU/F");
	tree->Branch("deltaetabb_etaOrdNoPU",&deltaetabb_etaOrdNoPU,"deltaetabb_etaOrdNoPU/F");
	tree->Branch("deltaphibb_etaOrdNoPU",&deltaphibb_etaOrdNoPU,"deltaphibb_etaOrdNoPU/F");
	tree->Branch("ptsqq_etaOrdNoPU",&ptsqq_etaOrdNoPU,"ptsqq_etaOrdNoPU/F");
	tree->Branch("ptsbb_etaOrdNoPU",&ptsbb_etaOrdNoPU,"ptsbb_etaOrdNoPU/F");
	tree->Branch("signeta_etaOrdNoPU",&signeta_etaOrdNoPU,"signeta_etaOrdNoPU/F");
	tree->Branch("q1Eta_etaOrdNoPU",&q1Eta_etaOrdNoPU,"q1Eta_etaOrdNoPU/F");
	tree->Branch("q1Phi_etaOrdNoPU",&q1Phi_etaOrdNoPU,"q1Phi_etaOrdNoPU/F");
	tree->Branch("q1Pt_etaOrdNoPU",&q1Pt_etaOrdNoPU,"q1Pt_etaOrdNoPU/F");
	tree->Branch("q2Eta_etaOrdNoPU",&q2Eta_etaOrdNoPU,"q2Eta_etaOrdNoPU/F");
	tree->Branch("q2Phi_etaOrdNoPU",&q2Phi_etaOrdNoPU,"q2Phi_etaOrdNoPU/F");
	tree->Branch("q2Pt_etaOrdNoPU",&q2Pt_etaOrdNoPU,"q2Pt_etaOrdNoPU/F");
	tree->Branch("b1Eta_etaOrdNoPU",&b1Eta_etaOrdNoPU,"b1Eta_etaOrdNoPU/F");
	tree->Branch("b1Phi_etaOrdNoPU",&b1Phi_etaOrdNoPU,"b1Phi_etaOrdNoPU/F");
	tree->Branch("b1Pt_etaOrdNoPU",&b1Pt_etaOrdNoPU,"b1Pt_etaOrdNoPU/F");
	tree->Branch("b2Eta_etaOrdNoPU",&b2Eta_etaOrdNoPU,"b2Eta_etaOrdNoPU/F");
	tree->Branch("b2Phi_etaOrdNoPU",&b2Phi_etaOrdNoPU,"b2Phi_etaOrdNoPU/F");
	tree->Branch("b2Pt_etaOrdNoPU",&b2Pt_etaOrdNoPU,"b2Pt_etaOrdNoPU/F");

	tree->Branch("q1deltaR_etaOrdNoPU",&q1deltaR_etaOrdNoPU,"q1deltaR_etaOrdNoPU/F");	
	tree->Branch("q2deltaR_etaOrdNoPU",&q2deltaR_etaOrdNoPU,"q2deltaR_etaOrdNoPU/F");	
	tree->Branch("b1deltaR_etaOrdNoPU",&b1deltaR_etaOrdNoPU,"b1deltaR_etaOrdNoPU/F");	
	tree->Branch("b2deltaR_etaOrdNoPU",&b2deltaR_etaOrdNoPU,"b2deltaR_etaOrdNoPU/F");	

	tree->Branch("mqq_etaPFOrd",&mqq_etaPFOrd,"mqq_etaPFOrd/F");
	tree->Branch("deltaetaqq_etaPFOrd",&deltaetaqq_etaPFOrd,"deltaetaqq_etaPFOrd/F");
	tree->Branch("deltaetabb_etaPFOrd",&deltaetabb_etaPFOrd,"deltaetabb_etaPFOrd/F");
	tree->Branch("deltaphibb_etaPFOrd",&deltaphibb_etaPFOrd,"deltaphibb_etaPFOrd/F");
	tree->Branch("ptsqq_etaPFOrd",&ptsqq_etaPFOrd,"ptsqq_etaPFOrd/F");
	tree->Branch("ptsbb_etaPFOrd",&ptsbb_etaPFOrd,"ptsbb_etaPFOrd/F");
	tree->Branch("signeta_etaPFOrd",&signeta_etaPFOrd,"signeta_etaPFOrd/F");
	tree->Branch("q1Eta_etaPFOrd",&q1Eta_etaPFOrd,"q1Eta_etaPFOrd/F");
	tree->Branch("q1Phi_etaPFOrd",&q1Phi_etaPFOrd,"q1Phi_etaPFOrd/F");
	tree->Branch("q1Pt_etaPFOrd",&q1Pt_etaPFOrd,"q1Pt_etaPFOrd/F");
	tree->Branch("q2Eta_etaPFOrd",&q2Eta_etaPFOrd,"q2Eta_etaPFOrd/F");
	tree->Branch("q2Phi_etaPFOrd",&q2Phi_etaPFOrd,"q2Phi_etaPFOrd/F");
	tree->Branch("q2Pt_etaPFOrd",&q2Pt_etaPFOrd,"q2Pt_etaPFOrd/F");
	tree->Branch("b1Eta_etaPFOrd",&b1Eta_etaPFOrd,"b1Eta_etaPFOrd/F");
	tree->Branch("b1Phi_etaPFOrd",&b1Phi_etaPFOrd,"b1Phi_etaPFOrd/F");
	tree->Branch("b1Pt_etaPFOrd",&b1Pt_etaPFOrd,"b1Pt_etaPFOrd/F");
	tree->Branch("b2Eta_etaPFOrd",&b2Eta_etaPFOrd,"b2Eta_etaPFOrd/F");
	tree->Branch("b2Phi_etaPFOrd",&b2Phi_etaPFOrd,"b2Phi_etaPFOrd/F");
	tree->Branch("b2Pt_etaPFOrd",&b2Pt_etaPFOrd,"b2Pt_etaPFOrd/F");

	tree->Branch("q1deltaR_etaPFOrd",&q1deltaR_etaPFOrd,"q1deltaR_etaPFOrd/F");	
	tree->Branch("q2deltaR_etaPFOrd",&q2deltaR_etaPFOrd,"q2deltaR_etaPFOrd/F");	
	tree->Branch("b1deltaR_etaPFOrd",&b1deltaR_etaPFOrd,"b1deltaR_etaPFOrd/F");	
	tree->Branch("b2deltaR_etaPFOrd",&b2deltaR_etaPFOrd,"b2deltaR_etaPFOrd/F");	
	
	tree->Branch("mqq_etaTrackOrd",&mqq_etaTrackOrd,"mqq_etaTrackOrd/F");
	tree->Branch("deltaetaqq_etaTrackOrd",&deltaetaqq_etaTrackOrd,"deltaetaqq_etaTrackOrd/F");
	tree->Branch("deltaetabb_etaTrackOrd",&deltaetabb_etaTrackOrd,"deltaetabb_etaTrackOrd/F");
	tree->Branch("deltaphibb_etaTrackOrd",&deltaphibb_etaTrackOrd,"deltaphibb_etaTrackOrd/F");
	tree->Branch("ptsqq_etaTrackOrd",&ptsqq_etaTrackOrd,"ptsqq_etaTrackOrd/F");
	tree->Branch("ptsbb_etaTrackOrd",&ptsbb_etaTrackOrd,"ptsbb_etaTrackOrd/F");
	tree->Branch("signeta_etaTrackOrd",&signeta_etaTrackOrd,"signeta_etaTrackOrd/F");
	tree->Branch("q1Eta_etaTrackOrd",&q1Eta_etaTrackOrd,"q1Eta_etaTrackOrd/F");
	tree->Branch("q1Phi_etaTrackOrd",&q1Phi_etaTrackOrd,"q1Phi_etaTrackOrd/F");
	tree->Branch("q1Pt_etaTrackOrd",&q1Pt_etaTrackOrd,"q1Pt_etaTrackOrd/F");
	tree->Branch("q2Eta_etaTrackOrd",&q2Eta_etaTrackOrd,"q2Eta_etaTrackOrd/F");
	tree->Branch("q2Phi_etaTrackOrd",&q2Phi_etaTrackOrd,"q2Phi_etaTrackOrd/F");
	tree->Branch("q2Pt_etaTrackOrd",&q2Pt_etaTrackOrd,"q2Pt_etaTrackOrd/F");
	tree->Branch("b1Eta_etaTrackOrd",&b1Eta_etaTrackOrd,"b1Eta_etaTrackOrd/F");
	tree->Branch("b1Phi_etaTrackOrd",&b1Phi_etaTrackOrd,"b1Phi_etaTrackOrd/F");
	tree->Branch("b1Pt_etaTrackOrd",&b1Pt_etaTrackOrd,"b1Pt_etaTrackOrd/F");
	tree->Branch("b2Eta_etaTrackOrd",&b2Eta_etaTrackOrd,"b2Eta_etaTrackOrd/F");
	tree->Branch("b2Phi_etaTrackOrd",&b2Phi_etaTrackOrd,"b2Phi_etaTrackOrd/F");
	tree->Branch("b2Pt_etaTrackOrd",&b2Pt_etaTrackOrd,"b2Pt_etaTrackOrd/F");

	tree->Branch("q1deltaR_etaTrackOrd",&q1deltaR_etaTrackOrd,"q1deltaR_etaTrackOrd/F");	
	tree->Branch("q2deltaR_etaTrackOrd",&q2deltaR_etaTrackOrd,"q2deltaR_etaTrackOrd/F");	
	tree->Branch("b1deltaR_etaTrackOrd",&b1deltaR_etaTrackOrd,"b1deltaR_etaTrackOrd/F");	
	tree->Branch("b2deltaR_etaTrackOrd",&b2deltaR_etaTrackOrd,"b2deltaR_etaTrackOrd/F");	
	
	tree->Branch("mqq_CSVL25Ord",&mqq_CSVL25Ord,"mqq_CSVL25Ord/F");
	tree->Branch("deltaetaqq_CSVL25Ord",&deltaetaqq_CSVL25Ord,"deltaetaqq_CSVL25Ord/F");
	tree->Branch("deltaetabb_CSVL25Ord",&deltaetabb_CSVL25Ord,"deltaetabb_CSVL25Ord/F");
	tree->Branch("deltaphibb_CSVL25Ord",&deltaphibb_CSVL25Ord,"deltaphibb_CSVL25Ord/F");
	tree->Branch("ptsqq_CSVL25Ord",&ptsqq_CSVL25Ord,"ptsqq_CSVL25Ord/F");
	tree->Branch("ptsbb_CSVL25Ord",&ptsbb_CSVL25Ord,"ptsbb_CSVL25Ord/F");
	tree->Branch("signeta_CSVL25Ord",&signeta_CSVL25Ord,"signeta_CSVL25Ord/F");
	tree->Branch("q1Eta_CSVL25Ord",&q1Eta_CSVL25Ord,"q1Eta_CSVL25Ord/F");
	tree->Branch("q1Phi_CSVL25Ord",&q1Phi_CSVL25Ord,"q1Phi_CSVL25Ord/F");
	tree->Branch("q1Pt_CSVL25Ord",&q1Pt_CSVL25Ord,"q1Pt_CSVL25Ord/F");
	tree->Branch("q2Eta_CSVL25Ord",&q2Eta_CSVL25Ord,"q2Eta_CSVL25Ord/F");
	tree->Branch("q2Phi_CSVL25Ord",&q2Phi_CSVL25Ord,"q2Phi_CSVL25Ord/F");
	tree->Branch("q2Pt_CSVL25Ord",&q2Pt_CSVL25Ord,"q2Pt_CSVL25Ord/F");
	tree->Branch("b1Eta_CSVL25Ord",&b1Eta_CSVL25Ord,"b1Eta_CSVL25Ord/F");
	tree->Branch("b1Phi_CSVL25Ord",&b1Phi_CSVL25Ord,"b1Phi_CSVL25Ord/F");
	tree->Branch("b1Pt_CSVL25Ord",&b1Pt_CSVL25Ord,"b1Pt_CSVL25Ord/F");
	tree->Branch("b2Eta_CSVL25Ord",&b2Eta_CSVL25Ord,"b2Eta_CSVL25Ord/F");
	tree->Branch("b2Phi_CSVL25Ord",&b2Phi_CSVL25Ord,"b2Phi_CSVL25Ord/F");
	tree->Branch("b2Pt_CSVL25Ord",&b2Pt_CSVL25Ord,"b2Pt_CSVL25Ord/F");

	tree->Branch("q1deltaR_CSVL25Ord",&q1deltaR_CSVL25Ord,"q1deltaR_CSVL25Ord/F");	
	tree->Branch("q2deltaR_CSVL25Ord",&q2deltaR_CSVL25Ord,"q2deltaR_CSVL25Ord/F");	
	tree->Branch("b1deltaR_CSVL25Ord",&b1deltaR_CSVL25Ord,"b1deltaR_CSVL25Ord/F");	
	tree->Branch("b2deltaR_CSVL25Ord",&b2deltaR_CSVL25Ord,"b2deltaR_CSVL25Ord/F");		
	
	tree->Branch("mqq_CSVL3Ord",&mqq_CSVL3Ord,"mqq_CSVL3Ord/F");
	tree->Branch("deltaetaqq_CSVL3Ord",&deltaetaqq_CSVL3Ord,"deltaetaqq_CSVL3Ord/F");
	tree->Branch("deltaetabb_CSVL3Ord",&deltaetabb_CSVL3Ord,"deltaetabb_CSVL3Ord/F");
	tree->Branch("deltaphibb_CSVL3Ord",&deltaphibb_CSVL3Ord,"deltaphibb_CSVL3Ord/F");
	tree->Branch("ptsqq_CSVL3Ord",&ptsqq_CSVL3Ord,"ptsqq_CSVL3Ord/F");
	tree->Branch("ptsbb_CSVL3Ord",&ptsbb_CSVL3Ord,"ptsbb_CSVL3Ord/F");
	tree->Branch("signeta_CSVL3Ord",&signeta_CSVL3Ord,"signeta_CSVL3Ord/F");
	tree->Branch("q1Eta_CSVL3Ord",&q1Eta_CSVL3Ord,"q1Eta_CSVL3Ord/F");
	tree->Branch("q1Phi_CSVL3Ord",&q1Phi_CSVL3Ord,"q1Phi_CSVL3Ord/F");
	tree->Branch("q1Pt_CSVL3Ord",&q1Pt_CSVL3Ord,"q1Pt_CSVL3Ord/F");
	tree->Branch("q2Eta_CSVL3Ord",&q2Eta_CSVL3Ord,"q2Eta_CSVL3Ord/F");
	tree->Branch("q2Phi_CSVL3Ord",&q2Phi_CSVL3Ord,"q2Phi_CSVL3Ord/F");
	tree->Branch("q2Pt_CSVL3Ord",&q2Pt_CSVL3Ord,"q2Pt_CSVL3Ord/F");
	tree->Branch("b1Eta_CSVL3Ord",&b1Eta_CSVL3Ord,"b1Eta_CSVL3Ord/F");
	tree->Branch("b1Phi_CSVL3Ord",&b1Phi_CSVL3Ord,"b1Phi_CSVL3Ord/F");
	tree->Branch("b1Pt_CSVL3Ord",&b1Pt_CSVL3Ord,"b1Pt_CSVL3Ord/F");
	tree->Branch("b2Eta_CSVL3Ord",&b2Eta_CSVL3Ord,"b2Eta_CSVL3Ord/F");
	tree->Branch("b2Phi_CSVL3Ord",&b2Phi_CSVL3Ord,"b2Phi_CSVL3Ord/F");
	tree->Branch("b2Pt_CSVL3Ord",&b2Pt_CSVL3Ord,"b2Pt_CSVL3Ord/F");	
	
	tree->Branch("mqq_CSVPFOrd",&mqq_CSVPFOrd,"mqq_CSVPFOrd/F");
	tree->Branch("deltaetaqq_CSVPFOrd",&deltaetaqq_CSVPFOrd,"deltaetaqq_CSVPFOrd/F");
	tree->Branch("deltaetabb_CSVPFOrd",&deltaetabb_CSVPFOrd,"deltaetabb_CSVPFOrd/F");
	tree->Branch("deltaphibb_CSVPFOrd",&deltaphibb_CSVPFOrd,"deltaphibb_CSVPFOrd/F");
	tree->Branch("ptsqq_CSVPFOrd",&ptsqq_CSVPFOrd,"ptsqq_CSVPFOrd/F");
	tree->Branch("ptsbb_CSVPFOrd",&ptsbb_CSVPFOrd,"ptsbb_CSVPFOrd/F");
	tree->Branch("signeta_CSVPFOrd",&signeta_CSVPFOrd,"signeta_CSVPFOrd/F");
	tree->Branch("q1Eta_CSVPFOrd",&q1Eta_CSVPFOrd,"q1Eta_CSVPFOrd/F");
	tree->Branch("q1Phi_CSVPFOrd",&q1Phi_CSVPFOrd,"q1Phi_CSVPFOrd/F");
	tree->Branch("q1Pt_CSVPFOrd",&q1Pt_CSVPFOrd,"q1Pt_CSVPFOrd/F");
	tree->Branch("q2Eta_CSVPFOrd",&q2Eta_CSVPFOrd,"q2Eta_CSVPFOrd/F");
	tree->Branch("q2Phi_CSVPFOrd",&q2Phi_CSVPFOrd,"q2Phi_CSVPFOrd/F");
	tree->Branch("q2Pt_CSVPFOrd",&q2Pt_CSVPFOrd,"q2Pt_CSVPFOrd/F");
	tree->Branch("b1Eta_CSVPFOrd",&b1Eta_CSVPFOrd,"b1Eta_CSVPFOrd/F");
	tree->Branch("b1Phi_CSVPFOrd",&b1Phi_CSVPFOrd,"b1Phi_CSVPFOrd/F");
	tree->Branch("b1Pt_CSVPFOrd",&b1Pt_CSVPFOrd,"b1Pt_CSVPFOrd/F");
	tree->Branch("b2Eta_CSVPFOrd",&b2Eta_CSVPFOrd,"b2Eta_CSVPFOrd/F");
	tree->Branch("b2Phi_CSVPFOrd",&b2Phi_CSVPFOrd,"b2Phi_CSVPFOrd/F");
	tree->Branch("b2Pt_CSVPFOrd",&b2Pt_CSVPFOrd,"b2Pt_CSVPFOrd/F");	

	tree->Branch("q1deltaR_CSVL3Ord",&q1deltaR_CSVL3Ord,"q1deltaR_CSVL3Ord/F");	
	tree->Branch("q2deltaR_CSVL3Ord",&q2deltaR_CSVL3Ord,"q2deltaR_CSVL3Ord/F");	
	tree->Branch("b1deltaR_CSVL3Ord",&b1deltaR_CSVL3Ord,"b1deltaR_CSVL3Ord/F");	
	tree->Branch("b2deltaR_CSVL3Ord",&b2deltaR_CSVL3Ord,"b2deltaR_CSVL3Ord/F");	

	tree->Branch("q1deltaR_CSVPFOrd",&q1deltaR_CSVPFOrd,"q1deltaR_CSVPFOrd/F");	
	tree->Branch("q2deltaR_CSVPFOrd",&q2deltaR_CSVPFOrd,"q2deltaR_CSVPFOrd/F");	
	tree->Branch("b1deltaR_CSVPFOrd",&b1deltaR_CSVPFOrd,"b1deltaR_CSVPFOrd/F");	
	tree->Branch("b2deltaR_CSVPFOrd",&b2deltaR_CSVPFOrd,"b2deltaR_CSVPFOrd/F");	

	tree->Branch("caloht",&caloht,"caloht/F");	
	tree->Branch("calomht",&calomht,"calomht/F");	
	tree->Branch("calomhtPhi",&calomhtPhi,"calomhtPhi/F");	
	tree->Branch("caloNoPUht",&caloNoPUht,"caloNoPUht/F");	
	tree->Branch("caloNoPUmht",&caloNoPUmht,"caloNoPUmht/F");	
	tree->Branch("caloNoPUmhtPhi",&caloNoPUmhtPhi,"caloNoPUmhtPhi/F");	
	tree->Branch("pfht",&pfht,"pfht/F");	
	tree->Branch("pfmht",&pfmht,"pfmht/F");	
	tree->Branch("pfmhtPhi",&pfmhtPhi,"pfmhtPhi/F");	

	tree->Branch("caloet",&caloet,"caloet/F");	
	tree->Branch("calomet",&calomet,"calomet/F");	
	tree->Branch("calometPhi",&calometPhi,"calometPhi/F");	

	tree->Branch("caloetClean",&caloetClean,"caloetClean/F");	
	tree->Branch("calometClean",&calometClean,"calometClean/F");	
	tree->Branch("calometPhiClean",&calometPhiClean,"calometPhiClean/F");	

	tree->Branch("caloetCleanUsingJetID",&caloetCleanUsingJetID,"caloetCleanUsingJetID/F");	
	tree->Branch("calometCleanUsingJetID",&calometCleanUsingJetID,"calometCleanUsingJetID/F");	
	tree->Branch("calometPhiCleanUsingJetID",&calometPhiCleanUsingJetID,"calometPhiCleanUsingJetID/F");	



	tree->Branch("pfet",&pfet,"pfet/F");	
	tree->Branch("pfmet",&pfmet,"pfmet/F");	
	tree->Branch("pfmetPhi",&pfmetPhi,"pfmetPhi/F");	

   
	tree->Branch("dRj4_4b",dRj4_4b,"dRj4_4b[3]/F");
	tree->Branch("Mjj_diffOrdered_4b",Mjj_diffOrdered_4b,"Mjj_diffOrdered_4b[6]/F");
	tree->Branch("Mjj_4b",Mjj_4b,"Mjj_4b[6]/F");



	tree->Branch("ncjets",&ncjets,"ncjets/I");
	tree->Branch("cjetPt",cjetPt,"cjetPt[ncjets]/F");
	tree->Branch("cjetEta",cjetEta,"cjetEta[ncjets]/F");
	tree->Branch("cjetPhi",cjetPhi,"cjetPhi[ncjets]/F");

	tree->Branch("nfwjets",&nfwjets,"nfwjets/I");
	tree->Branch("fwjetPt",fwjetPt,"fwjetPt[nfwjets]/F");
	tree->Branch("fwjetEta",fwjetEta,"fwjetEta[nfwjets]/F");
	tree->Branch("fwjetPhi",fwjetPhi,"fwjetPhi[nfwjets]/F");

	tree->Branch("ntaujets",&ntaujets,"ntaujets/I");
	tree->Branch("taujetPt",taujetPt,"taujetPt[ntaujets]/F");
	tree->Branch("taujetEta",taujetEta,"taujetEta[ntaujets]/F");
	tree->Branch("taujetPhi",taujetPhi,"taujetPhi[ntaujets]/F");

	tree->Branch("nmuons",&nmuons,"nmuons/I");
	tree->Branch("muonPt",muonPt,"muonPt[nmuons]/F");
	tree->Branch("muonEta",muonEta,"muonEta[nmuons]/F");
	tree->Branch("muonPhi",muonPhi,"muonPhi[nmuons]/F");


	tree->Branch("njets",&njets,"njets/I");
	tree->Branch("jetPt",jetPt,"jetPt[njets]/F");
	tree->Branch("jetEta",jetEta,"jetEta[njets]/F");
	tree->Branch("jetPhi",jetPhi,"jetPhi[njets]/F");

	tree->Branch("metPt",&metPt,"metPt/F");
	tree->Branch("etTot",&etTot,"etTot/F");
	tree->Branch("etMiss",&etMiss,"etMiss/F");
	tree->Branch("metEta",&metEta,"metEta/F");
	tree->Branch("metPhi",&metPhi,"metPhi/F");

	tree->Branch("mhtPt",&mhtPt,"mhtPt/F");
	tree->Branch("htTot",&htTot,"htTot/F");
	tree->Branch("htMiss",&htMiss,"htMiss/F");
	tree->Branch("mhtEta",&mhtEta,"mhtEta/F");
	tree->Branch("mhtPhi",&mhtPhi,"mhtPhi/F");

	tree->Branch("metPtGen",&metPtGen,"metPtGen/F");
	tree->Branch("metEtaGen",&metEtaGen,"metEtaGen/F");
	tree->Branch("metPhiGen",&metPhiGen,"metPhiGen/F");

	tree->Branch("hfring0",&hfring0,"hfring0/F");
	tree->Branch("hfring1",&hfring1,"hfring1/F");
	tree->Branch("hfring2",&hfring2,"hfring2/F");
	tree->Branch("hfring3",&hfring3,"hfring3/F");
	tree->Branch("hfring4",&hfring4,"hfring4/F");


	tree->Branch("pu",&pu,"pu/I");


//****************************************************
}



void
NtuplerVBF::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{


b1Eta_gen=0;b1Phi_gen=0;b1Pt_gen=0;b2Eta_gen=0;b2Phi_gen=0;b2Pt_gen=0;q1Eta_gen=0;q1Phi_gen=0;q1Pt_gen=0;q2Eta_gen=0;q2Phi_gen=0;q2Pt_gen=0;

mqq_etaOrd=0;deltaetaqq_etaOrd=0;deltaphibb_etaOrd=0;deltaetabb_etaOrd=0;ptsqq_etaOrd=0;ptsbb_etaOrd=0;signeta_etaOrd=0;q1Eta_etaOrd=0;q1Phi_etaOrd=0;q1Pt_etaOrd=0;q2Eta_etaOrd=0;q2Phi_etaOrd=0;q2Pt_etaOrd=0;b1Eta_etaOrd=0;b1Phi_etaOrd=0;b1Pt_etaOrd=0;b2Eta_etaOrd=0;b2Phi_etaOrd=0;b2Pt_etaOrd=0;
b1deltaR_etaOrd=0;b2deltaR_etaOrd=0;q1deltaR_etaOrd=0;q2deltaR_etaOrd=0;

mqq_etaOrdNoPU=0;deltaetaqq_etaOrdNoPU=0;deltaphibb_etaOrdNoPU=0;deltaetabb_etaOrdNoPU=0;ptsqq_etaOrdNoPU=0;ptsbb_etaOrdNoPU=0;signeta_etaOrdNoPU=0;q1Eta_etaOrdNoPU=0;q1Phi_etaOrdNoPU=0;q1Pt_etaOrdNoPU=0;q2Eta_etaOrdNoPU=0;q2Phi_etaOrdNoPU=0;q2Pt_etaOrdNoPU=0;b1Eta_etaOrdNoPU=0;b1Phi_etaOrdNoPU=0;b1Pt_etaOrdNoPU=0;b2Eta_etaOrdNoPU=0;b2Phi_etaOrdNoPU=0;b2Pt_etaOrdNoPU=0;
b1deltaR_etaOrdNoPU=0;b2deltaR_etaOrdNoPU=0;q1deltaR_etaOrdNoPU=0;q2deltaR_etaOrdNoPU=0;

mqq_etaPFOrd=0;deltaetaqq_etaPFOrd=0;deltaphibb_etaPFOrd=0;deltaetabb_etaPFOrd=0;ptsqq_etaPFOrd=0;ptsbb_etaPFOrd=0;signeta_etaPFOrd=0;q1Eta_etaPFOrd=0;q1Phi_etaPFOrd=0;q1Pt_etaPFOrd=0;q2Eta_etaPFOrd=0;q2Phi_etaPFOrd=0;q2Pt_etaPFOrd=0;b1Eta_etaPFOrd=0;b1Phi_etaPFOrd=0;b1Pt_etaPFOrd=0;b2Eta_etaPFOrd=0;b2Phi_etaPFOrd=0;b2Pt_etaPFOrd=0;
b1deltaR_etaPFOrd=0;b2deltaR_etaPFOrd=0;q1deltaR_etaPFOrd=0;q2deltaR_etaPFOrd=0;

mqq_etaTrackOrd=0;deltaetaqq_etaTrackOrd=0;deltaphibb_etaTrackOrd=0;deltaetabb_etaTrackOrd=0;ptsqq_etaTrackOrd=0;ptsbb_etaTrackOrd=0;signeta_etaTrackOrd=0;q1Eta_etaTrackOrd=0;q1Phi_etaTrackOrd=0;q1Pt_etaTrackOrd=0;q2Eta_etaTrackOrd=0;q2Phi_etaTrackOrd=0;q2Pt_etaTrackOrd=0;b1Eta_etaTrackOrd=0;b1Phi_etaTrackOrd=0;b1Pt_etaTrackOrd=0;b2Eta_etaTrackOrd=0;b2Phi_etaTrackOrd=0;b2Pt_etaTrackOrd=0;
b1deltaR_etaTrackOrd=0;b2deltaR_etaTrackOrd=0;q1deltaR_etaTrackOrd=0;q2deltaR_etaTrackOrd=0;

mqq_CSVL25Ord=0;deltaetaqq_CSVL25Ord=0;deltaphibb_CSVL25Ord=0;deltaetabb_CSVL25Ord=0;ptsqq_CSVL25Ord=0;ptsbb_CSVL25Ord=0;signeta_CSVL25Ord=0;q1Eta_CSVL25Ord=0;q1Phi_CSVL25Ord=0;q1Pt_CSVL25Ord=0;q2Eta_CSVL25Ord=0;q2Phi_CSVL25Ord=0;q2Pt_CSVL25Ord=0;b1Eta_CSVL25Ord=0;b1Phi_CSVL25Ord=0;b1Pt_CSVL25Ord=0;b2Eta_CSVL25Ord=0;b2Phi_CSVL25Ord=0;b2Pt_CSVL25Ord=0;
b1deltaR_CSVL25Ord=0;b2deltaR_CSVL25Ord=0;q1deltaR_CSVL25Ord=0;q2deltaR_CSVL25Ord=0;

mqq_CSVL3Ord=0;deltaetaqq_CSVL3Ord=0;deltaphibb_CSVL3Ord=0;deltaetabb_CSVL3Ord=0;ptsqq_CSVL3Ord=0;ptsbb_CSVL3Ord=0;signeta_CSVL3Ord=0;q1Eta_CSVL3Ord=0;q1Phi_CSVL3Ord=0;q1Pt_CSVL3Ord=0;q2Eta_CSVL3Ord=0;q2Phi_CSVL3Ord=0;q2Pt_CSVL3Ord=0;b1Eta_CSVL3Ord=0;b1Phi_CSVL3Ord=0;b1Pt_CSVL3Ord=0;b2Eta_CSVL3Ord=0;b2Phi_CSVL3Ord=0;b2Pt_CSVL3Ord=0;
b1deltaR_CSVL3Ord=0;b2deltaR_CSVL3Ord=0;q1deltaR_CSVL3Ord=0;q2deltaR_CSVL3Ord=0;

mqq_CSVPFOrd=0;deltaetaqq_CSVPFOrd=0;deltaphibb_CSVPFOrd=0;deltaetabb_CSVPFOrd=0;ptsqq_CSVPFOrd=0;ptsbb_CSVPFOrd=0;signeta_CSVPFOrd=0;q1Eta_CSVPFOrd=0;q1Phi_CSVPFOrd=0;q1Pt_CSVPFOrd=0;q2Eta_CSVPFOrd=0;q2Phi_CSVPFOrd=0;q2Pt_CSVPFOrd=0;b1Eta_CSVPFOrd=0;b1Phi_CSVPFOrd=0;b1Pt_CSVPFOrd=0;b2Eta_CSVPFOrd=0;b2Phi_CSVPFOrd=0;b2Pt_CSVPFOrd=0;
b1deltaR_CSVPFOrd=0;b2deltaR_CSVPFOrd=0;q1deltaR_CSVPFOrd=0;q2deltaR_CSVPFOrd=0;

caloht=0; calomht=0; caloNoPUht=0; caloNoPUmht=0; pfht=0; pfmht=0; 
caloet=0; calomet=0; pfet=0; pfmet=0; 

nevent=0;
ngenjets=0;
npfjets=0;
ntrackjets=0;
ncalojets=0;
ncaloNoPUjets=0;
nCSVL25=0;
nCSVL3=0;
nCSVPF=0;

   using namespace edm;
   using namespace reco;
   using namespace std;
//*****************************************************
nevent=iEvent.id().event();
nrun=iEvent.id().run();
nlumi=iEvent.id().luminosityBlock();
//*****************************************************

   Handle<edm::View<reco::GenJet> > gjH;
   iEvent.getByLabel(edm::InputTag("ak5GenJets"),gjH);
   if(!gjH.failedToGet())
   {
   const edm::View<reco::GenJet> & genjets = *gjH.product();
 
   ngenjets=0;
   for(edm::View<reco::GenJet>::const_iterator it = genjets.begin() ; it != genjets.end() ; it++)
   {
    if(it->pt()<10) continue;
	genjetPt[ngenjets]  = it->pt();
   	genjetEta[ngenjets] = it->eta();
   	genjetPhi[ngenjets] = it->phi();
   	ngenjets++;
   }
    
	b2Eta_gen	=	-1;
	b2Phi_gen	=	-1;
	b2Pt_gen	=	-1;

	b1Eta_gen	=	-1;
	b1Phi_gen	=	-1;
	b1Pt_gen	=	-1;

	q2Eta_gen	=	-1;
	q2Phi_gen	=	-1;
	q2Pt_gen	=	-1;
	
	q1Eta_gen	=	-1;
	q1Phi_gen	=	-1;
	q1Pt_gen	=	-1;

  }
  edm::Handle<vector<reco::GenParticle> > genParticlesH;
  iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesH);
  if(!genParticlesH.failedToGet())
  {  
  const vector<reco::GenParticle> & genParticles = *genParticlesH.product();
  
  for(vector<reco::GenParticle>::const_iterator it = genParticles.begin() ; it != genParticles.end() ; it++)
  {
  	if(abs(it->pdgId())==5 && it->mother()->pdgId()==25) 
  	{
  		if(it->pt()>b1Pt_gen)
  		{
		  	b2Eta_gen	=	b1Eta_gen;
		  	b2Phi_gen	=	b1Phi_gen;
		  	b2Pt_gen	=	b1Pt_gen;

		  	b1Eta_gen	=	it->eta();
		  	b1Phi_gen	=	it->phi();
		  	b1Pt_gen	=	it->pt();
  		}
  		else if(it->pt()>b2Pt_gen)
  		{
		  	b2Eta_gen	=	it->eta();
		  	b2Phi_gen	=	it->phi();
		  	b2Pt_gen	=	it->pt();
  		}
  	} 
  	else if( fabs(it->pdgId())<5 && fabs(it->pdgId())>0 && it->mother()->pt()==0 ) 
  	{
  		if(it->pt()>q1Pt_gen)
  		{
		  	q2Eta_gen	=	q1Eta_gen;
		  	q2Phi_gen	=	q1Phi_gen;
		  	q2Pt_gen	=	q1Pt_gen;  	
		  	
		  	q1Eta_gen	=	it->eta();
		  	q1Phi_gen	=	it->phi();
		  	q1Pt_gen	=	it->pt();  	
		}
		else if(it->pt()>q2Pt_gen) 
		{
		  	q2Eta_gen	=	it->eta();
		  	q2Phi_gen	=	it->phi();
		  	q2Pt_gen	=	it->pt();  	
		}
  	}
  }
  }		

{
Handle<edm::View<reco::PFJet> > PFjH;
iEvent.getByLabel(edm::InputTag("hltPFJetID"),PFjH);
if(!PFjH.failedToGet())
{
const edm::View<reco::PFJet> & pfjets = *PFjH.product();
   npfjets=0;
   for(edm::View<reco::PFJet>::const_iterator it = pfjets.begin() ; it != pfjets.end() ; it++)
   {
   	if(it->pt()>10)
   	{
	pfjetPt[npfjets]  = it->pt();
   	pfjetEta[npfjets] = it->eta();
   	pfjetPhi[npfjets] = it->phi();
   	npfjets++;
   	}
   }
}


}


{
Handle<edm::View<reco::TrackJet> > trackjH;
iEvent.getByLabel(edm::InputTag("hltAntiKT5TrackJets"),trackjH);
if(!trackjH.failedToGet())
{
const edm::View<reco::TrackJet> & trackjets = *trackjH.product();
   ntrackjets=0;
   for(edm::View<reco::TrackJet>::const_iterator it = trackjets.begin() ; it != trackjets.end() ; it++)
   {
   	if(it->pt()>10)
   	{
	trackjetPt[ntrackjets]  = it->pt();
   	trackjetEta[ntrackjets] = it->eta();
   	trackjetPhi[ntrackjets] = it->phi();
   	ntrackjets++;
   	}
   }
}


}
{
   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltJetNoPU"),jH);
   const edm::View<reco::CaloJet> & caloNoPUjets = *jH.product();
if(!jH.failedToGet())
{ 
   ncaloNoPUjets=0;
   for(edm::View<reco::CaloJet>::const_iterator it = caloNoPUjets.begin() ; it != caloNoPUjets.end() ; it++)
   {
   	if(it->pt()>10 && fabs(it->eta())<2.4)
   	{
	caloNoPUjetPt[ncaloNoPUjets]  = it->pt();
   	caloNoPUjetEta[ncaloNoPUjets] = it->eta();
   	caloNoPUjetPhi[ncaloNoPUjets] = it->phi();
   	ncaloNoPUjets++;
   	}
   }   
 }
}
{
   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltAK4CaloJetsCorrectedIDPassed"),jH);
   const edm::View<reco::CaloJet> & calojets = *jH.product();
 
   ncalojets=0;
   for(edm::View<reco::CaloJet>::const_iterator it = calojets.begin() ; it != calojets.end() ; it++)
   {
   	if(it->pt()>10)
   	{
	calojetPt[ncalojets]  = it->pt();
   	calojetEta[ncalojets] = it->eta();
   	calojetPhi[ncalojets] = it->phi();
   	ncalojets++;
   	}
   }
}

//Eta ordered 
{  
   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;
   

   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltAK4CaloJetsCorrectedIDPassed"),jH);
   const edm::View<reco::CaloJet> & calojets = *jH.product();
 

unsigned int nJet=0;
   for(edm::View<reco::CaloJet>::const_iterator jet = calojets.begin() ; jet != calojets.end() && nJet<nMax; jet++){
       float value=jet->eta();
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
     if(nJet>=4)
     {
      sort(sorted.begin(),sorted.end(),comparator);

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     q1 = calojets[sorted[3].second].p4();
     b1 = calojets[sorted[2].second].p4();
     b2 = calojets[sorted[1].second].p4();
     q2 = calojets[sorted[0].second].p4();
    
   
	mqq_etaOrd = (q1+q2).M();
	deltaetaqq_etaOrd = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_etaOrd = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_etaOrd = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_etaOrd = (q1+q2).Pt();
	ptsbb_etaOrd = (b1+b2).Pt();
	signeta_etaOrd = q1.Eta()*q2.Eta();
 
	q1Eta_etaOrd	=	q1.eta();
	q1Phi_etaOrd	=	q1.phi();
	q1Pt_etaOrd	=	q1.pt();  	

	q2Eta_etaOrd	=	q2.eta();
	q2Phi_etaOrd	=	q2.phi();
	q2Pt_etaOrd	=	q2.pt();  	

	b1Eta_etaOrd	=	b1.eta();
	b1Phi_etaOrd	=	b1.phi();
	b1Pt_etaOrd	=	b1.pt();  	

	b2Eta_etaOrd	=	b2.eta();
	b2Phi_etaOrd	=	b2.phi();
	b2Pt_etaOrd	=	b2.pt();  

	q1deltaR_etaOrd	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_etaOrd	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_etaOrd	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_etaOrd	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 	
	}
}	


//Eta ordered  NOPU
{
   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltJetNoPU"),jH);
   const edm::View<reco::CaloJet> & calojets = *jH.product();

if(!jH.failedToGet())
{ 
unsigned int nJet=0;
   for(edm::View<reco::CaloJet>::const_iterator jet = calojets.begin() ; jet != calojets.end() && nJet<nMax; jet++){
       float value=jet->eta();
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
     if(nJet>=4)
     {
     sort(sorted.begin(),sorted.end(),comparator);

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     q1 = calojets[sorted[3].second].p4();
     b1 = calojets[sorted[2].second].p4();
     b2 = calojets[sorted[1].second].p4();
     q2 = calojets[sorted[0].second].p4();
    
   
	mqq_etaOrdNoPU = (q1+q2).M();
	deltaetaqq_etaOrdNoPU = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_etaOrdNoPU = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_etaOrdNoPU = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_etaOrdNoPU = (q1+q2).Pt();
	ptsbb_etaOrdNoPU = (b1+b2).Pt();
	signeta_etaOrdNoPU = q1.Eta()*q2.Eta();
 
	q1Eta_etaOrdNoPU	=	q1.eta();
	q1Phi_etaOrdNoPU	=	q1.phi();
	q1Pt_etaOrdNoPU	=	q1.pt();  	

	q2Eta_etaOrdNoPU	=	q2.eta();
	q2Phi_etaOrdNoPU	=	q2.phi();
	q2Pt_etaOrdNoPU	=	q2.pt();  	

	b1Eta_etaOrdNoPU	=	b1.eta();
	b1Phi_etaOrdNoPU	=	b1.phi();
	b1Pt_etaOrdNoPU	=	b1.pt();  	

	b2Eta_etaOrdNoPU	=	b2.eta();
	b2Phi_etaOrdNoPU	=	b2.phi();
	b2Pt_etaOrdNoPU	=	b2.pt();  

	q1deltaR_etaOrdNoPU	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_etaOrdNoPU	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_etaOrdNoPU	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_etaOrdNoPU	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 	
}
}
}


//PFEta ordered 
{

   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::PFJet> > PFjH;
iEvent.getByLabel(edm::InputTag("hltAK4PFJets"),PFjH);
if(!PFjH.failedToGet())
{
   const edm::View<reco::PFJet> & pfjets = *PFjH.product();

unsigned int nJet=0;
   for(edm::View<reco::PFJet>::const_iterator jet = pfjets.begin() ; jet != pfjets.end() && nJet<nMax; jet++){
       float value=jet->eta();
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
          if(nJet>=4)
     {
     sort(sorted.begin(),sorted.end(),comparator);

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     q1 = pfjets[sorted[3].second].p4();
     b1 = pfjets[sorted[2].second].p4();
     b2 = pfjets[sorted[1].second].p4();
     q2 = pfjets[sorted[0].second].p4();
        
   
	mqq_etaPFOrd = (q1+q2).M();
	deltaetaqq_etaPFOrd = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_etaPFOrd = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_etaPFOrd = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_etaPFOrd = (q1+q2).Pt();
	ptsbb_etaPFOrd = (b1+b2).Pt();
	signeta_etaPFOrd = q1.Eta()*q2.Eta();
 
	q1Eta_etaPFOrd	=	q1.eta();
	q1Phi_etaPFOrd	=	q1.phi();
	q1Pt_etaPFOrd	=	q1.pt();  	

	q2Eta_etaPFOrd	=	q2.eta();
	q2Phi_etaPFOrd	=	q2.phi();
	q2Pt_etaPFOrd	=	q2.pt();  	

	b1Eta_etaPFOrd	=	b1.eta();
	b1Phi_etaPFOrd	=	b1.phi();
	b1Pt_etaPFOrd	=	b1.pt();  	

	b2Eta_etaPFOrd	=	b2.eta();
	b2Phi_etaPFOrd	=	b2.phi();
	b2Pt_etaPFOrd	=	b2.pt();  

	q1deltaR_etaPFOrd	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_etaPFOrd	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_etaPFOrd	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_etaPFOrd	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
	
	}
	}	

}
   
   
//TrackEta ordered 
{

   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::TrackJet> > TrackjH;
iEvent.getByLabel(edm::InputTag("hltAntiKT5TrackJets"),TrackjH);
if(!TrackjH.failedToGet())
{
   const edm::View<reco::TrackJet> & Trackjets = *TrackjH.product();

unsigned int nJet=0;
   for(edm::View<reco::TrackJet>::const_iterator jet = Trackjets.begin() ; jet != Trackjets.end() && nJet<nMax; jet++){
       float value=jet->eta();
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
          if(nJet>=4)
     {
     sort(sorted.begin(),sorted.end(),comparator);

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     q1 = Trackjets[sorted[3].second].p4();
     b1 = Trackjets[sorted[2].second].p4();
     b2 = Trackjets[sorted[1].second].p4();
     q2 = Trackjets[sorted[0].second].p4();
        
   
	mqq_etaTrackOrd = (q1+q2).M();
	deltaetaqq_etaTrackOrd = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_etaTrackOrd = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_etaTrackOrd = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_etaTrackOrd = (q1+q2).Pt();
	ptsbb_etaTrackOrd = (b1+b2).Pt();
	signeta_etaTrackOrd = q1.Eta()*q2.Eta();
 
	q1Eta_etaTrackOrd	=	q1.eta();
	q1Phi_etaTrackOrd	=	q1.phi();
	q1Pt_etaTrackOrd	=	q1.pt();  	

	q2Eta_etaTrackOrd	=	q2.eta();
	q2Phi_etaTrackOrd	=	q2.phi();
	q2Pt_etaTrackOrd	=	q2.pt();  	

	b1Eta_etaTrackOrd	=	b1.eta();
	b1Phi_etaTrackOrd	=	b1.phi();
	b1Pt_etaTrackOrd	=	b1.pt();  	

	b2Eta_etaTrackOrd	=	b2.eta();
	b2Phi_etaTrackOrd	=	b2.phi();
	b2Pt_etaTrackOrd	=	b2.pt();  

	q1deltaR_etaTrackOrd	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_etaTrackOrd	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_etaTrackOrd	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_etaTrackOrd	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
	
	}
	}	

}
   
      
//CSVL25 ordered 
{
   const unsigned int nMax(4);
   vector<Jpair> sorted;
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::CaloJet> > jH;
iEvent.getByLabel(edm::InputTag("hltSelector8CentralJetsL1FastJet"),jH);
const edm::View<reco::CaloJet> & bjets = *jH.product();
   

  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltCombinedSecondaryVertexL25BJetTagsHbbVBF",jetTags);


//  Handle<reco::JetTagCollection>  RecoJetTagshandle;
//  iEvent.getByLabel("hlt3combinedSecondaryVertexBJetTags", RecoJetTagshandle);
//  Handle<reco::JetTagCollection>  RecoJetTagshandle_L25;
//  iEvent.getByLabel("hlt25combinedSecondaryVertexBJetTags", RecoJetTagshandle_L25);
//  
//const reco::JetTagCollection & bTags = *(RecoJetTagshandle.product());
//const reco::JetTagCollection & bTags_L25 = *(RecoJetTagshandle_L25.product());

// for(unsigned int i=0;i<bTags.size() && i<N_jet;i++)
//  {
//	 CSV_jet[i]=bTags[i].second;
//	 CSV_L25_jet[i]=bTags_L25[i].second;
//  }
//  
//  const reco::JetTagCollection & bTags = *(RecoJetTagshandle.product());

unsigned int nJet=0;
//if(bjets.size()>=4 && jetTags->size()>=4)
{
//     for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<N_jet ); ++jet) {
       for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<bjets.size() && nJet<N_jet); ++jet) {       
       float value = jet->second;
       sorted.push_back(make_pair(value,nJet));
       ++nJet;
     }
     sort(sorted.begin(),sorted.end(),comparatorInv);		

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();

		nCSVL25=nJet;
		for(unsigned int i=0; i<nJet ; i++){
		   CSVL25[i]=sorted[i].first;
		   CSVL25_jetPt[i]=bjets[sorted[i].second].p4().pt();
		   CSVL25_jetEta[i]=bjets[sorted[i].second].p4().eta();
		   CSVL25_jetPhi[i]=bjets[sorted[i].second].p4().phi();
       }
           
    if(nJet>=4) {
	mqq_CSVL25Ord = (q1+q2).M();
	deltaetaqq_CSVL25Ord = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_CSVL25Ord = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_CSVL25Ord = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_CSVL25Ord = (q1+q2).Pt();
	ptsbb_CSVL25Ord = (b1+b2).Pt();
	signeta_CSVL25Ord = q1.Eta()*q2.Eta();
 
	q1Eta_CSVL25Ord	=	q1.eta();
	q1Phi_CSVL25Ord	=	q1.phi();
	q1Pt_CSVL25Ord	=	q1.pt();  	

	q2Eta_CSVL25Ord	=	q2.eta();
	q2Phi_CSVL25Ord	=	q2.phi();
	q2Pt_CSVL25Ord	=	q2.pt();  	

	b1Eta_CSVL25Ord	=	b1.eta();
	b1Phi_CSVL25Ord	=	b1.phi();
	b1Pt_CSVL25Ord	=	b1.pt();  	

	b2Eta_CSVL25Ord	=	b2.eta();
	b2Phi_CSVL25Ord	=	b2.phi();
	b2Pt_CSVL25Ord	=	b2.pt();  

	q1deltaR_CSVL25Ord	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_CSVL25Ord	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_CSVL25Ord	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_CSVL25Ord	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
        }
}		
}

//CSVL3 ordered 
{
   const unsigned int nMax(4);
   vector<Jpair> sorted;
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::CaloJet> > jH;
iEvent.getByLabel(edm::InputTag("hltSelector8CentralJetsL1FastJet"),jH);
const edm::View<reco::CaloJet> & bjets = *jH.product();
   

  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltL3CombinedSecondaryVertexBJetTags",jetTags);

//if(bjets.size()>=4 && jetTags->size()>=4)
{
unsigned int nJet=0;
       for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<bjets.size() && nJet<N_jet); ++jet) {
         float value = jet->second;
       sorted.push_back(make_pair(value,nJet));
       ++nJet;
     }
     if(nJet>=1) sort(sorted.begin(),sorted.end(),comparatorInv);
     
//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();

		nCSVL3=nJet;
		for(unsigned int i=0; i<nJet ; i++){
		   CSVL3[i]=sorted[i].first;
		   CSVL3_jetPt[i]=bjets[sorted[i].second].p4().pt();
		   CSVL3_jetEta[i]=bjets[sorted[i].second].p4().eta();
		   CSVL3_jetPhi[i]=bjets[sorted[i].second].p4().phi();
       }
   
     if(nJet>=4)
     { 
	mqq_CSVL3Ord = (q1+q2).M();
	deltaetaqq_CSVL3Ord = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_CSVL3Ord = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_CSVL3Ord = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_CSVL3Ord = (q1+q2).Pt();
	ptsbb_CSVL3Ord = (b1+b2).Pt();
	signeta_CSVL3Ord = q1.Eta()*q2.Eta();
 
	q1Eta_CSVL3Ord	=	q1.eta();
	q1Phi_CSVL3Ord	=	q1.phi();
	q1Pt_CSVL3Ord	=	q1.pt();  	

	q2Eta_CSVL3Ord	=	q2.eta();
	q2Phi_CSVL3Ord	=	q2.phi();
	q2Pt_CSVL3Ord	=	q2.pt();  	

	b1Eta_CSVL3Ord	=	b1.eta();
	b1Phi_CSVL3Ord	=	b1.phi();
	b1Pt_CSVL3Ord	=	b1.pt();  	

	b2Eta_CSVL3Ord	=	b2.eta();
	b2Phi_CSVL3Ord	=	b2.phi();
	b2Pt_CSVL3Ord	=	b2.pt();  	

	q1deltaR_CSVL3Ord	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_CSVL3Ord	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_CSVL3Ord	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_CSVL3Ord	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
      }
}
}

//CSVPF ordered 
{
   const unsigned int nMax(4);
   vector<Jpair> sorted;
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::PFJet> > jH;
iEvent.getByLabel(edm::InputTag("hltPFJetID"),jH);
const edm::View<reco::PFJet> & bjets = *jH.product();
   
  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltCombinedSecondaryVertexBJetTagsPF",jetTags);

//if(bjets.size()>=4 && jetTags->size()>=4)
{
unsigned int nJet=0;
       for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<bjets.size() && nJet<N_jet); ++jet) {
         float value = jet->second;
       if(jet->first->pt()<10) continue;
       sorted.push_back(make_pair(value,nJet));
       ++nJet;
     }
     if(nJet>=1) sort(sorted.begin(),sorted.end(),comparatorInv);
     
//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();

		nCSVPF=nJet;
		for(unsigned int i=0; i<nJet ; i++){
		   CSVPF[i]=sorted[i].first;
		   CSVPF_jetPt[i]=bjets[sorted[i].second].p4().pt();
		   CSVPF_jetEta[i]=bjets[sorted[i].second].p4().eta();
		   CSVPF_jetPhi[i]=bjets[sorted[i].second].p4().phi();
       }
       
     if(nJet>=4)
     { 
	mqq_CSVPFOrd = (q1+q2).M();
	deltaetaqq_CSVPFOrd = std::abs(q1.Eta()-q2.Eta());
	deltaetabb_CSVPFOrd = std::abs(b1.Eta()-b2.Eta());
	deltaphibb_CSVPFOrd = deltaPhi(b1.Phi(),b2.Phi());
	ptsqq_CSVPFOrd = (q1+q2).Pt();
	ptsbb_CSVPFOrd = (b1+b2).Pt();
	signeta_CSVPFOrd = q1.Eta()*q2.Eta();
 
	q1Eta_CSVPFOrd	=	q1.eta();
	q1Phi_CSVPFOrd	=	q1.phi();
	q1Pt_CSVPFOrd	=	q1.pt();  	

	q2Eta_CSVPFOrd	=	q2.eta();
	q2Phi_CSVPFOrd	=	q2.phi();
	q2Pt_CSVPFOrd	=	q2.pt();  	

	b1Eta_CSVPFOrd	=	b1.eta();
	b1Phi_CSVPFOrd	=	b1.phi();
	b1Pt_CSVPFOrd	=	b1.pt();  	

	b2Eta_CSVPFOrd	=	b2.eta();
	b2Phi_CSVPFOrd	=	b2.phi();
	b2Pt_CSVPFOrd	=	b2.pt();  	

	q1deltaR_CSVPFOrd	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_CSVPFOrd	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_CSVPFOrd	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_CSVPFOrd	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
      }
}
}

Handle<edm::View<reco::MET> > calometH;
iEvent.getByLabel(edm::InputTag("hltMet"),calometH);

Handle<edm::View<reco::MET> > calometHClean;
iEvent.getByLabel(edm::InputTag("hltMetClean"),calometHClean);

Handle<edm::View<reco::MET> > calometHCleanUsingJetID;
iEvent.getByLabel(edm::InputTag("hltMetCleanUsingJetID"),calometHCleanUsingJetID);

Handle<edm::View<reco::MET> > pfmetH;
iEvent.getByLabel(edm::InputTag("hltPFMETProducer"),pfmetH);


Handle<edm::View<reco::MET> > calomhtH;
iEvent.getByLabel(edm::InputTag("hltCaloHtMhtProducer"),calomhtH);

Handle<edm::View<reco::MET> > caloNoPUmetH;
iEvent.getByLabel(edm::InputTag("hltCaloNoPUHtMhtProducer"),caloNoPUmetH);

Handle<edm::View<reco::MET> > pfmhtH;
iEvent.getByLabel(edm::InputTag("hltPFHtMhtProducer"),pfmhtH);

caloet=calometH->begin()->sumEt();
calomet=calometH->begin()->pt();
calometPhi=calometH->begin()->phi();

caloetClean=calometHClean->begin()->sumEt();
calometClean=calometHClean->begin()->pt();
calometPhiClean=calometHClean->begin()->phi();

caloetCleanUsingJetID=calometHCleanUsingJetID->begin()->sumEt();
calometCleanUsingJetID=calometHCleanUsingJetID->begin()->pt();
calometPhiCleanUsingJetID=calometHCleanUsingJetID->begin()->phi();


pfet=pfmetH->begin()->sumEt();
pfmet=pfmetH->begin()->pt();
pfmetPhi=pfmetH->begin()->phi();


caloht=calomhtH->begin()->sumEt();
calomht=calomhtH->begin()->pt();
calomhtPhi=calomhtH->begin()->phi();

caloNoPUht=caloNoPUmetH->begin()->sumEt();
caloNoPUmht=caloNoPUmetH->begin()->pt();
caloNoPUmhtPhi=caloNoPUmetH->begin()->phi();

pfht=pfmhtH->begin()->sumEt();
pfmht=pfmhtH->begin()->pt(); 
pfmhtPhi=pfmhtH->begin()->phi(); 

    TLorentzVector q1, q2, q3, q4;
	if(nCSVL3>0)    {q1.SetPtEtaPhiM(CSVL3_jetPt[0],CSVL3_jetEta[0],CSVPF_jetPhi[0],0);}
	if(nCSVL3>1)    {q2.SetPtEtaPhiM(CSVL3_jetPt[1],CSVL3_jetEta[1],CSVPF_jetPhi[1],0);}
	if(nCSVL3>2)    {q3.SetPtEtaPhiM(CSVL3_jetPt[2],CSVL3_jetEta[2],CSVPF_jetPhi[2],0);}
	if(nCSVL3>3)    {q4.SetPtEtaPhiM(CSVL3_jetPt[3],CSVL3_jetEta[3],CSVPF_jetPhi[3],0);}
	double mjj[6], diff[6], dR[3];
    mjj[0] = (q1+q2).M();
    mjj[1] = (q1+q3).M();
    mjj[2] = (q1+q4).M();
    mjj[3] = (q2+q3).M();
    mjj[4] = (q2+q4).M();
    mjj[5] = (q3+q4).M();
	dR[0]= q4.DeltaR(q1);
	dR[1]= q4.DeltaR(q2);
	dR[2]= q4.DeltaR(q3);

	mjj_map a, b, c;
	
	for(int i =0; i< 6; i++) {a[mjj[i]]=i; diff[i] = TMath::Abs(mjj[i] -125.); b[diff[i]] = i;}
	 for(int j =0; j< 3; j++) c[dR[j]]=j;
	 int c0 =0;
	 for (mjj_map::reverse_iterator iJet=a.rbegin(); iJet!=a.rend(); ++iJet)
         {
          Mjj_4b[c0]=mjj[iJet->second];//iJet->first;
          c0++;
	}
	 int c1 =0;
         for (mjj_map::iterator iJet1=b.begin(); iJet1!=b.end(); ++iJet1)
         {
          Mjj_diffOrdered_4b[c1]=mjj[iJet1->second];//iJet->first;
                              c1++;
                                      }
	 int c2 =0;
         for (mjj_map::iterator iJet2=c.begin(); iJet2!=c.end(); ++iJet2)
         {
          dRj4_4b[c2]=dR[iJet2->second];//iJet->first;
                              c2++;
                                      }


/// L1 extra particles


  nfwjets=0;
  ntaujets=0;
  ncjets=0;
  njets=0;
  nmuons=0;
  pu=-1;

  edm::Handle< edm::View<reco::Candidate> > jetParticleCentralH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","Central","ReRunningL1"),jetParticleCentralH);
  const edm::View<reco::Candidate> & jetParticleCentrals = *jetParticleCentralH.product();
  
  edm::Handle< edm::View<reco::Candidate> > jetParticleForwardH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","Forward","ReRunningL1"),jetParticleForwardH);
  const edm::View<reco::Candidate> & jetParticleForwards = *jetParticleForwardH.product();

  edm::Handle< edm::View<reco::Candidate> > muonParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","","ReRunningL1"),muonParticleH);
  const edm::View<reco::Candidate> & muonParticles = *muonParticleH.product();

//  edm::Handle< edm::View<reco::Candidate> > jetParticleTauH;
//  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","Tau","ReRunningL1"),jetParticleTauH);
//  const edm::View<reco::Candidate> & jetParticleTaus = *jetParticleTauH.product();

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > metParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","MET","ReRunningL1"),metParticleH);
  const edm::View<l1extra::L1EtMissParticle> & metParticle = *metParticleH.product();

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > mhtParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","MHT","ReRunningL1"),mhtParticleH);
  const edm::View<l1extra::L1EtMissParticle> & mhtParticle = *mhtParticleH.product();

//  edm::Handle< edm::View<reco::Candidate> > 
  edm::Handle< edm::View<reco::Candidate> > pfMetH;
  iEvent.getByLabel(edm::InputTag("genMetTrue"), pfMetH);
  const edm::View<reco::Candidate> & pfMets = *pfMetH.product();
//  cout<<"pfMet="<<pfMets.begin()->et()<<endl;

  edm::Handle < std::vector < PileupSummaryInfo  > > puHandle;
  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puHandle);
  const PileupSummaryInfo & puObj = *puHandle->begin();
  if(puHandle->size() > 1) cout<<"********* Check PileUp stuff !! *******" <<endl;
  pu = puObj.getTrueNumInteractions();

  edm::Handle< edm::View<l1extra::L1HFRings> > hfringParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticlesUCT","","ReRunningL1"),hfringParticleH);
  const edm::View<l1extra::L1HFRings> & hfringParticle = *hfringParticleH.product();

	  hfring0=0;
	  hfring1=0;
	  hfring2=0;
	  hfring3=0;
	  hfring4=0;

  if(hfringParticle.size()<=0){
	cout<<endl<<"hfringParticle.size<=0 !!"<<endl;}
  else{
	  hfring0=hfringParticle.begin()->hfEtSum(l1extra::L1HFRings::kRing1PosEta);
	  hfring1=hfringParticle.begin()->hfEtSum(l1extra::L1HFRings::kRing1NegEta);
	  hfring2=hfringParticle.begin()->hfEtSum(l1extra::L1HFRings::kRing2PosEta);
	  hfring3=hfringParticle.begin()->hfEtSum(l1extra::L1HFRings::kRing2NegEta);
//	  hfring3=hfringParticle.begin()->hfEtSum(l1extra::L1HFRings::kNumRings);
  }

  if(metParticle.size()<=0) {cout<<endl<<"metParticle.size<=0 !!"<<endl; return;}
  if(mhtParticle.size()<=0) {cout<<endl<<"mhtParticle.size<=0 !!"<<endl; return;}

  metPt=metParticle.begin()->pt();
  etTot=metParticle.begin()->etTotal();
  etMiss=metParticle.begin()->etMiss();
  metEta=metParticle.begin()->eta();
  metPhi=metParticle.begin()->phi();

  mhtPt=mhtParticle.begin()->pt();
  htTot=mhtParticle.begin()->etTotal();
  htMiss=mhtParticle.begin()->etMiss();
  mhtEta=mhtParticle.begin()->eta();
  mhtPhi=mhtParticle.begin()->phi();

  metPtGen=pfMets.begin()->pt();
  metEtaGen=pfMets.begin()->eta();
  metPhiGen=pfMets.begin()->phi();


  for(edm::View<reco::Candidate>::const_iterator muon=muonParticles.begin(); muon!=muonParticles.end(); muon++ )
  {
  	if(nmuons<N_muon)
  	{  	
	  	muonPt[nmuons]=muon->pt();
	  	muonEta[nmuons]=muon->eta();
	  	muonPhi[nmuons]=muon->phi();
	  	nmuons++;
	  	  	}
  	else
  	{
  	cout<<endl<<"N_muon>=nmuons !!"<<endl;
  	}
  }
  
  for(edm::View<reco::Candidate>::const_iterator cjet=jetParticleCentrals.begin(); cjet!=jetParticleCentrals.end(); cjet++ )
  {
  	if(ncjets<N_jet)
  	{  	
	  	cjetPt[ncjets]=cjet->pt();
	  	cjetEta[ncjets]=cjet->eta();
	  	cjetPhi[ncjets]=cjet->phi();
	  	ncjets++;

	  	jetPt[njets]=cjet->pt();
	  	jetEta[njets]=cjet->eta();
	  	jetPhi[njets]=cjet->phi();
	  	njets++;
	  	  	}
  	else
  	{
  	cout<<endl<<"N_jet>=ncjets !!"<<endl;
  	}
  }
  
  for(edm::View<reco::Candidate>::const_iterator fwjet=jetParticleForwards.begin(); fwjet!=jetParticleForwards.end(); fwjet++ )
  {
  	if(nfwjets<N_jet)
  	{  	
	  	fwjetPt[nfwjets]=fwjet->pt();
	  	fwjetEta[nfwjets]=fwjet->eta();
	  	fwjetPhi[nfwjets]=fwjet->phi();
	  	nfwjets++;

	  	jetPt[njets]=fwjet->pt();
	  	jetEta[njets]=fwjet->eta();
	  	jetPhi[njets]=fwjet->phi();
	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=nfwjets !!"<<endl;
  	}
  }




 tree->Fill();
}


//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerVBF);
