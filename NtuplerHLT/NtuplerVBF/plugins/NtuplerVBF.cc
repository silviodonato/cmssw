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

//************************************************************************
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#define N_jet       1000  
//************************************************************************

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


int nevent;

int ngenjets;
float genjetPt[N_jet],genjetEta[N_jet],genjetPhi[N_jet];

int ncalojets;
float calojetPt[N_jet],calojetEta[N_jet],calojetPhi[N_jet];

int ncaloNoPUjets;
float caloNoPUjetPt[N_jet],caloNoPUjetEta[N_jet],caloNoPUjetPhi[N_jet];

int ncalocentraljets;
float calocentraljetPt[N_jet],calocentraljetEta[N_jet],calocentraljetPhi[N_jet];


int npfcentraljets;
float pfcentraljetPt[N_jet],pfcentraljetEta[N_jet],pfcentraljetPhi[N_jet];

int npfjets;
float pfjetPt[N_jet],pfjetEta[N_jet],pfjetPhi[N_jet];

int ntrackcentraljets;
float trackcentraljetPt[N_jet],trackcentraljetEta[N_jet],trackcentraljetPhi[N_jet];

int ntrackjets;
float trackjetPt[N_jet],trackjetEta[N_jet],trackjetPhi[N_jet];

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

float CSV1_L25,CSV2_L25,CSV3_L25;

float CSV1_L3,CSV2_L3,CSV3_L3;

float CSV1_PF,CSV2_PF,CSV3_PF;

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

  //************************************
     
};

NtuplerVBF::NtuplerVBF(const edm::ParameterSet& iConfig)
{
	tree=file->make<TTree>("tree","tree");
	tree->Branch("nevent",&nevent,"nevent/I");

	tree->Branch("ngenjets",&ngenjets,"ngenjets/I");
	tree->Branch("genjetPt",genjetPt,"genjetPt[ngenjets]/F");
	tree->Branch("genjetEta",genjetEta,"genjetEta[ngenjets]/F");
	tree->Branch("genjetPhi",genjetPhi,"genjetPhi[ngenjets]/F");

	tree->Branch("ncalojets",&ncalojets,"ncalojets/I");
	tree->Branch("calojetPt",calojetPt,"calojetPt[ncalojets]/F");
	tree->Branch("calojetEta",calojetEta,"calojetEta[ncalojets]/F");
	tree->Branch("calojetPhi",calojetPhi,"calojetPhi[ncalojets]/F");

	tree->Branch("ncalocentraljets",&ncalocentraljets,"ncalocentraljets/I");
	tree->Branch("calocentraljetPt",calocentraljetPt,"calocentraljetPt[ncalocentraljets]/F");
	tree->Branch("calocentraljetEta",calocentraljetEta,"calocentraljetEta[ncalocentraljets]/F");
	tree->Branch("calocentraljetPhi",calocentraljetPhi,"calocentraljetPhi[ncalocentraljets]/F");

	tree->Branch("npfjets",&npfjets,"npfjets/I");
	tree->Branch("pfjetPt",pfjetPt,"pfjetPt[npfjets]/F");
	tree->Branch("pfjetEta",pfjetEta,"pfjetEta[npfjets]/F");
	tree->Branch("pfjetPhi",pfjetPhi,"pfjetPhi[npfjets]/F");

	tree->Branch("npfcentraljets",&npfcentraljets,"npfcentraljets/I");
	tree->Branch("pfcentraljetPt",pfcentraljetPt,"pfcentraljetPt[npfcentraljets]/F");
	tree->Branch("pfcentraljetEta",pfcentraljetEta,"pfcentraljetEta[npfcentraljets]/F");
	tree->Branch("pfcentraljetPhi",pfcentraljetPhi,"pfcentraljetPhi[npfcentraljets]/F");

	tree->Branch("ntrackjets",&ntrackjets,"ntrackjets/I");
	tree->Branch("trackjetPt",trackjetPt,"trackjetPt[ntrackjets]/F");
	tree->Branch("trackjetEta",trackjetEta,"trackjetEta[ntrackjets]/F");
	tree->Branch("trackjetPhi",trackjetPhi,"trackjetPhi[ntrackjets]/F");

	tree->Branch("ntrackcentraljets",&ntrackcentraljets,"ntrackcentraljets/I");
	tree->Branch("trackcentraljetPt",trackcentraljetPt,"trackcentraljetPt[ntrackcentraljets]/F");
	tree->Branch("trackcentraljetEta",trackcentraljetEta,"trackcentraljetEta[ntrackcentraljets]/F");
	tree->Branch("trackcentraljetPhi",trackcentraljetPhi,"trackcentraljetPhi[ntrackcentraljets]/F");
		
	tree->Branch("ncaloNoPUjets",&ncaloNoPUjets,"ncaloNoPUjets/I");
	tree->Branch("caloNoPUjetPt",caloNoPUjetPt,"caloNoPUjetPt[ncaloNoPUjets]/F");
	tree->Branch("caloNoPUjetEta",caloNoPUjetEta,"caloNoPUjetEta[ncaloNoPUjets]/F");
	tree->Branch("caloNoPUjetPhi",caloNoPUjetPhi,"caloNoPUjetPhi[ncaloNoPUjets]/F");


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

	
	
		
	
	tree->Branch("CSV1_L25",&CSV1_L25,"CSV1_L25/F");	
	tree->Branch("CSV2_L25",&CSV2_L25,"CSV2_L25/F");	
	tree->Branch("CSV3_L25",&CSV3_L25,"CSV3_L25/F");	

	tree->Branch("CSV1_L3",&CSV1_L3,"CSV1_L3/F");	
	tree->Branch("CSV2_L3",&CSV2_L3,"CSV2_L3/F");	
	tree->Branch("CSV3_L3",&CSV3_L3,"CSV3_L3/F");	

	tree->Branch("CSV1_PF",&CSV1_PF,"CSV1_PF/F");	
	tree->Branch("CSV2_PF",&CSV2_PF,"CSV2_PF/F");	
	tree->Branch("CSV3_PF",&CSV3_PF,"CSV3_PF/F");	

   
//****************************************************
}



void
NtuplerVBF::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

nevent=0;

//ngenjets=0;
//genjetPt[N_jet]=0,genjetEta[N_jet]=0,genjetPhi[N_jet]=0;

//ncalojets=0;
//calojetPt[N_jet]=0,calojetEta[N_jet]=0,calojetPhi[N_jet]=0;

//ncaloNoPUjets=0;
//caloNoPUjetPt[N_jet]=0,caloNoPUjetEta[N_jet]=0,caloNoPUjetPhi[N_jet]=0;

//ncalocentraljets=0;
//calocentraljetPt[N_jet]=0,calocentraljetEta[N_jet]=0,calocentraljetPhi[N_jet]=0;


//npfcentraljets=0;
//pfcentraljetPt[N_jet]=0,pfcentraljetEta[N_jet]=0,pfcentraljetPhi[N_jet]=0;

//npfjets=0;
//pfjetPt[N_jet]=0,pfjetEta[N_jet]=0,pfjetPhi[N_jet]=0;


b1Eta_gen=0,b1Phi_gen=0,b1Pt_gen=0,b2Eta_gen=0,b2Phi_gen=0,b2Pt_gen=0,q1Eta_gen=0,q1Phi_gen=0,q1Pt_gen=0,q2Eta_gen=0,q2Phi_gen=0,q2Pt_gen=0;

mqq_etaOrd=0,deltaetaqq_etaOrd=0,deltaphibb_etaOrd=0,deltaetabb_etaOrd=0,ptsqq_etaOrd=0,ptsbb_etaOrd=0,signeta_etaOrd=0,q1Eta_etaOrd=0,q1Phi_etaOrd=0,q1Pt_etaOrd=0,q2Eta_etaOrd=0,q2Phi_etaOrd=0,q2Pt_etaOrd=0,b1Eta_etaOrd=0,b1Phi_etaOrd=0,b1Pt_etaOrd=0,b2Eta_etaOrd=0,b2Phi_etaOrd=0,b2Pt_etaOrd=0;
b1deltaR_etaOrd=0,b2deltaR_etaOrd=0,q1deltaR_etaOrd=0,q2deltaR_etaOrd=0;

mqq_etaOrdNoPU=0,deltaetaqq_etaOrdNoPU=0,deltaphibb_etaOrdNoPU=0,deltaetabb_etaOrdNoPU=0,ptsqq_etaOrdNoPU=0,ptsbb_etaOrdNoPU=0,signeta_etaOrdNoPU=0,q1Eta_etaOrdNoPU=0,q1Phi_etaOrdNoPU=0,q1Pt_etaOrdNoPU=0,q2Eta_etaOrdNoPU=0,q2Phi_etaOrdNoPU=0,q2Pt_etaOrdNoPU=0,b1Eta_etaOrdNoPU=0,b1Phi_etaOrdNoPU=0,b1Pt_etaOrdNoPU=0,b2Eta_etaOrdNoPU=0,b2Phi_etaOrdNoPU=0,b2Pt_etaOrdNoPU=0;
b1deltaR_etaOrdNoPU=0,b2deltaR_etaOrdNoPU=0,q1deltaR_etaOrdNoPU=0,q2deltaR_etaOrdNoPU=0;

mqq_etaPFOrd=0,deltaetaqq_etaPFOrd=0,deltaphibb_etaPFOrd=0,deltaetabb_etaPFOrd=0,ptsqq_etaPFOrd=0,ptsbb_etaPFOrd=0,signeta_etaPFOrd=0,q1Eta_etaPFOrd=0,q1Phi_etaPFOrd=0,q1Pt_etaPFOrd=0,q2Eta_etaPFOrd=0,q2Phi_etaPFOrd=0,q2Pt_etaPFOrd=0,b1Eta_etaPFOrd=0,b1Phi_etaPFOrd=0,b1Pt_etaPFOrd=0,b2Eta_etaPFOrd=0,b2Phi_etaPFOrd=0,b2Pt_etaPFOrd=0;
b1deltaR_etaPFOrd=0,b2deltaR_etaPFOrd=0,q1deltaR_etaPFOrd=0,q2deltaR_etaPFOrd=0;

mqq_etaTrackOrd=0,deltaetaqq_etaTrackOrd=0,deltaphibb_etaTrackOrd=0,deltaetabb_etaTrackOrd=0,ptsqq_etaTrackOrd=0,ptsbb_etaTrackOrd=0,signeta_etaTrackOrd=0,q1Eta_etaTrackOrd=0,q1Phi_etaTrackOrd=0,q1Pt_etaTrackOrd=0,q2Eta_etaTrackOrd=0,q2Phi_etaTrackOrd=0,q2Pt_etaTrackOrd=0,b1Eta_etaTrackOrd=0,b1Phi_etaTrackOrd=0,b1Pt_etaTrackOrd=0,b2Eta_etaTrackOrd=0,b2Phi_etaTrackOrd=0,b2Pt_etaTrackOrd=0;
b1deltaR_etaTrackOrd=0,b2deltaR_etaTrackOrd=0,q1deltaR_etaTrackOrd=0,q2deltaR_etaTrackOrd=0;

mqq_CSVL25Ord=0,deltaetaqq_CSVL25Ord=0,deltaphibb_CSVL25Ord=0,deltaetabb_CSVL25Ord=0,ptsqq_CSVL25Ord=0,ptsbb_CSVL25Ord=0,signeta_CSVL25Ord=0,q1Eta_CSVL25Ord=0,q1Phi_CSVL25Ord=0,q1Pt_CSVL25Ord=0,q2Eta_CSVL25Ord=0,q2Phi_CSVL25Ord=0,q2Pt_CSVL25Ord=0,b1Eta_CSVL25Ord=0,b1Phi_CSVL25Ord=0,b1Pt_CSVL25Ord=0,b2Eta_CSVL25Ord=0,b2Phi_CSVL25Ord=0,b2Pt_CSVL25Ord=0;
b1deltaR_CSVL25Ord=0,b2deltaR_CSVL25Ord=0,q1deltaR_CSVL25Ord=0,q2deltaR_CSVL25Ord=0;

mqq_CSVL3Ord=0,deltaetaqq_CSVL3Ord=0,deltaphibb_CSVL3Ord=0,deltaetabb_CSVL3Ord=0,ptsqq_CSVL3Ord=0,ptsbb_CSVL3Ord=0,signeta_CSVL3Ord=0,q1Eta_CSVL3Ord=0,q1Phi_CSVL3Ord=0,q1Pt_CSVL3Ord=0,q2Eta_CSVL3Ord=0,q2Phi_CSVL3Ord=0,q2Pt_CSVL3Ord=0,b1Eta_CSVL3Ord=0,b1Phi_CSVL3Ord=0,b1Pt_CSVL3Ord=0,b2Eta_CSVL3Ord=0,b2Phi_CSVL3Ord=0,b2Pt_CSVL3Ord=0;
b1deltaR_CSVL3Ord=0,b2deltaR_CSVL3Ord=0,q1deltaR_CSVL3Ord=0,q2deltaR_CSVL3Ord=0;

mqq_CSVPFOrd=0,deltaetaqq_CSVPFOrd=0,deltaphibb_CSVPFOrd=0,deltaetabb_CSVPFOrd=0,ptsqq_CSVPFOrd=0,ptsbb_CSVPFOrd=0,signeta_CSVPFOrd=0,q1Eta_CSVPFOrd=0,q1Phi_CSVPFOrd=0,q1Pt_CSVPFOrd=0,q2Eta_CSVPFOrd=0,q2Phi_CSVPFOrd=0,q2Pt_CSVPFOrd=0,b1Eta_CSVPFOrd=0,b1Phi_CSVPFOrd=0,b1Pt_CSVPFOrd=0,b2Eta_CSVPFOrd=0,b2Phi_CSVPFOrd=0,b2Pt_CSVPFOrd=0;
b1deltaR_CSVPFOrd=0,b2deltaR_CSVPFOrd=0,q1deltaR_CSVPFOrd=0,q2deltaR_CSVPFOrd=0;


CSV1_L25=0,CSV2_L25=0,CSV3_L25=0;

CSV1_L3=0,CSV2_L3=0,CSV3_L3=0;

CSV1_PF=0,CSV2_PF=0,CSV3_PF=0;

nevent=0;
ngenjets=0;
npfjets=0;
npfcentraljets=0;

ntrackjets=0;
ntrackcentraljets=0;

ncalojets=0;
ncalocentraljets=0;
ncaloNoPUjets=0;
   using namespace edm;
   using namespace reco;
   using namespace std;
//*****************************************************
nevent=iEvent.id().event();
//*****************************************************

   Handle<edm::View<reco::GenJet> > gjH;
   iEvent.getByLabel(edm::InputTag("ak5GenJets"),gjH);
   if(!gjH.failedToGet())
   {
   const edm::View<reco::GenJet> & genjets = *gjH.product();
 
   ngenjets=0;
   for(edm::View<reco::GenJet>::const_iterator it = genjets.begin() ; it != genjets.end() ; it++)
   {
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

cout<<"1\n";
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

cout<<"2\n";


{
Handle<edm::View<reco::PFJet> > PFjH;
iEvent.getByLabel(edm::InputTag("hltPFJetID"),PFjH);
if(!PFjH.failedToGet())
{
const edm::View<reco::PFJet> & pfjets = *PFjH.product();
cout<<"2.5\n";
   npfcentraljets=0;
   for(edm::View<reco::PFJet>::const_iterator it = pfjets.begin() ; it != pfjets.end() ; it++)
   {
   	if(it->pt()>10 && fabs(it->eta())<2.4)
   	{
	pfcentraljetPt[npfcentraljets]  = it->pt();
   	pfcentraljetEta[npfcentraljets] = it->eta();
   	pfcentraljetPhi[npfcentraljets] = it->phi();
   	npfcentraljets++;
   	}
   }


cout<<"2.5\n";
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
cout<<"2.5\n";
   ntrackcentraljets=0;
   for(edm::View<reco::TrackJet>::const_iterator it = trackjets.begin() ; it != trackjets.end() ; it++)
   {
   	if(it->pt()>10 && fabs(it->eta())<2.4)
   	{
	trackcentraljetPt[ntrackcentraljets]  = it->pt();
   	trackcentraljetEta[ntrackcentraljets] = it->eta();
   	trackcentraljetPhi[ntrackcentraljets] = it->phi();
   	ntrackcentraljets++;
   	}
   }


cout<<"2.5\n";
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
   iEvent.getByLabel(edm::InputTag("hltCaloJetL1FastJetCorrected"),jH);
   const edm::View<reco::CaloJet> & calocentraljets = *jH.product();
 

cout<<"3\n";
   ncalocentraljets=0;
   for(edm::View<reco::CaloJet>::const_iterator it = calocentraljets.begin() ; it != calocentraljets.end() ; it++)
   {
   	if(it->pt()>10 && fabs(it->eta())<2.4)
   	{
	calocentraljetPt[ncalocentraljets]  = it->pt();
   	calocentraljetEta[ncalocentraljets] = it->eta();
   	calocentraljetPhi[ncalocentraljets] = it->phi();
   	ncalocentraljets++;
   	}
   }
}

{
   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltJetNoPU"),jH);
   const edm::View<reco::CaloJet> & caloNoPUjets = *jH.product();
if(!jH.failedToGet())
{ 

cout<<"3\n";
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
   iEvent.getByLabel(edm::InputTag("hltCaloJetL1FastJetCorrected"),jH);
   const edm::View<reco::CaloJet> & calojets = *jH.product();
 

cout<<"3\n";
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
cout<<"4\n";

//   Handle<TCollection> jets;
//   iEvent.getByLabel("hltCaloJetL1FastJetCorrected",jets);

 
cout<<"5\n";

//Eta ordered 
{  
   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;
   

   Handle<edm::View<reco::CaloJet> > jH;
   iEvent.getByLabel(edm::InputTag("hltCaloJetL1FastJetCorrected"),jH);
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
cout<<"6\n";

//PFEta ordered 
{

   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::PFJet> > PFjH;
iEvent.getByLabel(edm::InputTag("hltAK4PFJetL1FastL2L3CorrectedNoPU"),PFjH);
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
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::CaloJet> > jH;
iEvent.getByLabel(edm::InputTag("hltBLifetimeL25JetsHbbVBF"),jH);
const edm::View<reco::CaloJet> & bjets = *jH.product();
   

cout<<"6a\n";
  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltCombinedSecondaryVertexL25BJetTagsHbbVBF",jetTags);


cout<<"jH="<<jH.isValid()<<"\n";
cout<<"jH="<<jH.failedToGet()<<"\n";

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

cout<<"jetTags="<<jetTags.isValid()<<"\n";
cout<<"jetTags="<<jetTags.failedToGet()<<"\n";
//cout<<"jetTags(add)="<<*jetTags<<"\n";
unsigned int nJet=0;
cout<<"6b\n";
//if(bjets.size()>=4 && jetTags->size()>=4)
{
     for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<nMax ); ++jet) {
	cout<<"nMax="<<nMax<<"\n";
         float value = jet->second;
	cout<<"nMax="<<nMax<<"\n";
       sorted[nJet] = make_pair(value,nJet);
	cout<<"nMax="<<nMax<<"\n";
       ++nJet;
     }
cout<<"6c\n";
     sort(sorted.begin(),sorted.end(),comparatorInv);
     if(nJet>=1) CSV1_L25 = sorted[0].first;		
     if(nJet>=2) CSV2_L25 = sorted[1].first;		
     if(nJet>=3) CSV3_L25 = sorted[2].first;
		
cout<<"6d\n";

//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();
    
cout<<"6e\n";
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
cout<<"6e\n";

	q1deltaR_CSVL25Ord	=	min(deltaR(q1.eta(),q1.phi(),q1Eta_gen,q1Phi_gen),deltaR(q1.eta(),q1.phi(),q2Eta_gen,q2Phi_gen));  	
	q2deltaR_CSVL25Ord	=	min(deltaR(q2.eta(),q2.phi(),q1Eta_gen,q1Phi_gen),deltaR(q2.eta(),q2.phi(),q2Eta_gen,q2Phi_gen));  	
	b1deltaR_CSVL25Ord	=	min(deltaR(b1.eta(),b1.phi(),b1Eta_gen,b1Phi_gen),deltaR(b1.eta(),b1.phi(),b2Eta_gen,b2Phi_gen));  	
	b2deltaR_CSVL25Ord	=	min(deltaR(b2.eta(),b2.phi(),b1Eta_gen,b1Phi_gen),deltaR(b2.eta(),b2.phi(),b2Eta_gen,b2Phi_gen)); 
        }
}		
}

cout<<"7\n";



//CSVL3 ordered 
{
   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::CaloJet> > jH;
iEvent.getByLabel(edm::InputTag("hltBLifetimeL25JetsHbbVBF"),jH);
const edm::View<reco::CaloJet> & bjets = *jH.product();
   

  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltL3CombinedSecondaryVertexBJetTags",jetTags);

cout<<"7a\n";
//if(bjets.size()>=4 && jetTags->size()>=4)
{
unsigned int nJet=0;
       for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<bjets.size() && nJet<nMax); ++jet) {
         float value = jet->second;
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
     cout<<"7b\n";
     if(nJet>=1) sort(sorted.begin(),sorted.end(),comparatorInv);
     cout<<"7c\n";
     if(nJet>=1) CSV1_L3 = sorted[0].first;		
     if(nJet>=2) CSV2_L3 = sorted[1].first;		
     if(nJet>=3) CSV3_L3 = sorted[2].first;

     cout<<"7e\n";
     
//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();

     cout<<"7f\n";
   
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
     cout<<"7g\n";

}
}



//CSVPF ordered 
{
   const unsigned int nMax(4);
   vector<Jpair> sorted(nMax);
   vector<TRef> jetRefs(nMax);
   Particle::LorentzVector b1,b2,q1,q2;

Handle<edm::View<reco::PFJet> > jH;
iEvent.getByLabel(edm::InputTag("hltPFJetID"),jH);
const edm::View<reco::PFJet> & bjets = *jH.product();
   

  Handle<JetTagCollection> jetTags;
  iEvent.getByLabel("hltCombinedSecondaryVertexBJetTagsPF",jetTags);

cout<<"7a\n";
//if(bjets.size()>=4 && jetTags->size()>=4)
{
unsigned int nJet=0;
       for (JetTagCollection::const_iterator jet = jetTags->begin(); (jet!=jetTags->end()&&nJet<bjets.size() && nJet<nMax); ++jet) {
         float value = jet->second;
       sorted[nJet] = make_pair(value,nJet);
       ++nJet;
     }
     cout<<"7b\n";
     if(nJet>=1) sort(sorted.begin(),sorted.end(),comparatorInv);
     cout<<"7c\n";
     if(nJet>=1) CSV1_PF = sorted[0].first;		
     if(nJet>=2) CSV2_PF = sorted[1].first;		
     if(nJet>=3) CSV3_PF = sorted[2].first;

     cout<<"7e\n";
     
//     for (unsigned int i=0; i<nMax; ++i) {
//       jetRefs[i]=TRef(jH,sorted[i].second);
//     }
     if(nJet>=1) b1 = bjets[sorted[0].second].p4();
     if(nJet>=2) b2 = bjets[sorted[1].second].p4();
     if(nJet>=3) q1 = bjets[sorted[2].second].p4();
     if(nJet>=4) q2 = bjets[sorted[3].second].p4();

     cout<<"7f\n";
   
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
     cout<<"7g\n";

}
}

 
cout<<"8\n";

 tree->Fill();
}


//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerVBF);
