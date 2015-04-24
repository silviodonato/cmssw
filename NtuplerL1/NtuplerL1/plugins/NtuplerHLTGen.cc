// -*- C++ -*-
//
// Package:    NtuplerHLTGen/NtuplerHLTGen
// Class:      NtuplerHLTGen
// 
/**\class NtuplerHLTGen NtuplerHLTGen.cc NtuplerHLTGen/NtuplerHLTGen/plugins/NtuplerHLTGen.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  root
//         Created:  Tue, 15 Apr 2014 14:59:49 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "DataFormats/L1Trigger/interface/L1EmParticle.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"

#include "DataFormats/L1Trigger/interface/L1HFRings.h"

#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"

#include "DataFormats/Math/interface/deltaR.h"


#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/TrackJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include <DataFormats/RecoCandidate/interface/RecoEcalCandidate.h>

#include <DataFormats/PatCandidates/interface/MET.h>
#include <DataFormats/PatCandidates/interface/Jet.h>
#include <DataFormats/PatCandidates/interface/Electron.h>

//************************************************************************
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#define N_jet       20  
#define N_muon      10  
#define N_jetGen       1000  
//************************************************************************

float sumpt(float pt1,float phi1,float pt2,float phi2){
    float px1 = pt1 * cos(phi1);
    float py1 = pt1 * sin(phi1);
    float px2 = pt2 * cos(phi2);
    float py2 = pt2 * sin(phi2);
    return std::sqrt((px1+px2)*(px1+px2)+(py1+py2)*(py1+py2));
}

//
// class declaration
//

class NtuplerHLTGen : public edm::EDAnalyzer {
   public:
      explicit NtuplerHLTGen(const edm::ParameterSet&);
      ~NtuplerHLTGen();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      
      const float GenPtThreshold = 10;
      int nevent,nlumi,nrun;

      int ncjets,cjetMatched[N_jet];
      float cjetPt[N_jet],cjetEta[N_jet],cjetPhi[N_jet];

      int nfwjets,fwjetMatched[N_jet];
      float fwjetPt[N_jet],fwjetEta[N_jet],fwjetPhi[N_jet];

      int ntaujets,taujetMatched[N_jet];
      float taujetPt[N_jet],taujetEta[N_jet],taujetPhi[N_jet];

      int nisotaujets,isotaujetMatched[N_jet];
      float isotaujetPt[N_jet],isotaujetEta[N_jet],isotaujetPhi[N_jet];

      int njets,jetMatched[N_jet];
      float jetPt[N_jet],jetEta[N_jet],jetPhi[N_jet];

      int nmuons;
      float muonPt[2*N_muon],muonEta[2*N_muon],muonPhi[2*N_muon];

      int nisoegammas;
      float isoegammaPt[N_jet],isoegammaEta[N_jet],isoegammaPhi[N_jet];

      int negammas;
      float egammaPt[N_jet],egammaEta[N_jet],egammaPhi[N_jet];


      int njetsGen,jetGenCMatched[N_jetGen],jetGenFwMatched[N_jetGen];
      float jetPtGen[N_jetGen],jetEtaGen[N_jetGen],jetPhiGen[N_jetGen];

      float metPt,etTot,etMiss,metEta,metPhi;

      float mhtPt,htTot,htMiss,mhtEta,mhtPhi;

      float metPtGen,metEtaGen,metPhiGen;

      int pu;
	
      float hfring1, hfring2, hfring3, hfring4, hfring0;

      float HPt,HEta,HPhi;
      float b1Pt,b1Eta,b1Phi;
      float b2Pt,b2Eta,b2Phi;
      float WPt,WEta,WPhi;
      float ePt,eEta,ePhi;
      float muPt,muEta,muPhi;
      float vPt,vEta,vPhi;

    float pfmet, pfmetPhi, pfmht, pfmhtPhi; 
    float patmet, patmetPhi; 

    int nCSVPF;
    float CSVPF[N_jet], CSVPF_jetPt[N_jet],CSVPF_jetEta[N_jet],CSVPF_jetPhi[N_jet];

    int npfjets;
    float pfjetPt[N_jet],pfjetEta[N_jet],pfjetPhi[N_jet];

    int neleWP75s;
    float eleWP75Pt[N_jet],eleWP75Eta[N_jet],eleWP75Phi[N_jet];

    int neleWP85s;
    float eleWP85Pt[N_jet],eleWP85Eta[N_jet],eleWP85Phi[N_jet];

    int neleWPTights;
    float eleWPTightPt[N_jet],eleWPTightEta[N_jet],eleWPTightPhi[N_jet];

    int neleWPLooses;
    float eleWPLoosePt[N_jet],eleWPLooseEta[N_jet],eleWPLoosePhi[N_jet];

    int nnoelepfjets;
    float noelepfjetPt[N_jet],noelepfjetEta[N_jet],noelepfjetPhi[N_jet];

    float WPtMHT_reco, WPt_reco, HPt_reco;

    int npateles;
    float patelePt[N_jet],pateleEta[N_jet],patelePhi[N_jet],pateleMVAIso[N_jet];

    int npatjets;
    float patjetPt[N_jet],patjetEta[N_jet],patjetPhi[N_jet],patjetCSV[N_jet];

    float genPVz, PVz, offlinePVz;

    
	TTree *tree;
  	edm::Service<TFileService> file;	

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

//    triggerTriggerFilterObjectWithRefs_hltJetFilterSingleTopEle20__TEST.
//triggerTriggerFilterObjectWithRefs_hltPFJetForBtagSelector__TEST.


NtuplerHLTGen::NtuplerHLTGen(const edm::ParameterSet& iConfig)

{

	tree=file->make<TTree>("tree","tree");
	tree->Branch("nevent",&nevent,"nevent/I");
	tree->Branch("nlumi",&nlumi,"nlumi/I");
	tree->Branch("nrun",&nrun,"nrun/I");

    tree->Branch("nCSVPF",&nCSVPF,"nCSVPF/I");
    tree->Branch("CSVPF",CSVPF,"CSVPF[nCSVPF]/F");
    tree->Branch("CSVPF_jetPt",CSVPF_jetPt,"CSVPF_jetPt[nCSVPF]/F");
    tree->Branch("CSVPF_jetEta",CSVPF_jetEta,"CSVPF_jetEta[nCSVPF]/F");
    tree->Branch("CSVPF_jetPhi",CSVPF_jetPhi,"CSVPF_jetPhi[nCSVPF]/F");

    tree->Branch("pfmet",&pfmet,"pfmet/F");
    tree->Branch("pfmetPhi",&pfmetPhi,"pfmetPhi/F"); 

    tree->Branch("patmet",&patmet,"patmet/F");
    tree->Branch("patmetPhi",&patmetPhi,"patmetPhi/F"); 

    tree->Branch("pfmht",&pfmht,"pfmht/F");
    tree->Branch("pfmhtPhi",&pfmhtPhi,"pfmhtPhi/F"); 

    tree->Branch("npfjets",&npfjets,"npfjets/I");
    tree->Branch("pfjetPt",pfjetPt,"pfjetPt[npfjets]/F");
    tree->Branch("pfjetEta",pfjetEta,"pfjetEta[npfjets]/F");
    tree->Branch("pfjetPhi",pfjetPhi,"pfjetPhi[npfjets]/F");

    tree->Branch("neleWP85s",&neleWP85s,"neleWP85s/I");
    tree->Branch("eleWP85Pt",eleWP85Pt,"eleWP85Pt[neleWP85s]/F");
    tree->Branch("eleWP85Eta",eleWP85Eta,"eleWP85Eta[neleWP85s]/F");
    tree->Branch("eleWP85Phi",eleWP85Phi,"eleWP85Phi[neleWP85s]/F");

    tree->Branch("neleWP75s",&neleWP75s,"neleWP75s/I");
    tree->Branch("eleWP75Pt",eleWP75Pt,"eleWP75Pt[neleWP75s]/F");
    tree->Branch("eleWP75Eta",eleWP75Eta,"eleWP75Eta[neleWP75s]/F");
    tree->Branch("eleWP75Phi",eleWP75Phi,"eleWP75Phi[neleWP75s]/F");

    tree->Branch("neleWPLooses",&neleWPLooses,"neleWPLooses/I");
    tree->Branch("eleWPLoosePt",eleWPLoosePt,"eleWPLoosePt[neleWPLooses]/F");
    tree->Branch("eleWPLooseEta",eleWPLooseEta,"eleWPLooseEta[neleWPLooses]/F");
    tree->Branch("eleWPLoosePhi",eleWPLoosePhi,"eleWPLoosePhi[neleWPLooses]/F");

    tree->Branch("neleWPTights",&neleWPTights,"neleWPTights/I");
    tree->Branch("eleWPTightPt",eleWPTightPt,"eleWPTightPt[neleWPTights]/F");
    tree->Branch("eleWPTightEta",eleWPTightEta,"eleWPTightEta[neleWPTights]/F");
    tree->Branch("eleWPTightPhi",eleWPTightPhi,"eleWPTightPhi[neleWPTights]/F");

    tree->Branch("nnoelepfjets",&nnoelepfjets,"nnoelepfjets/I");
    tree->Branch("noelepfjetPt",noelepfjetPt,"noelepfjetPt[nnoelepfjets]/F");
    tree->Branch("noelepfjetEta",noelepfjetEta,"noelepfjetEta[nnoelepfjets]/F");
    tree->Branch("noelepfjetPhi",noelepfjetPhi,"noelepfjetPhi[nnoelepfjets]/F");

    tree->Branch("HPt_reco",&HPt_reco,"HPt_reco/F");
    tree->Branch("WPt_reco",&WPt_reco,"WPt_reco/F");
    tree->Branch("WPtMHT_reco",&WPtMHT_reco,"WPtMHT_reco/F");

	tree->Branch("HPt",&HPt,"HPt/F");
	tree->Branch("HEta",&HEta,"HEta/F");
	tree->Branch("HPhi",&HPhi,"HPhi/F");

	tree->Branch("b1Pt",&b1Pt,"b1Pt/F");
	tree->Branch("b1Eta",&b1Eta,"b1Eta/F");
	tree->Branch("b1Phi",&b1Phi,"b1Phi/F");

	tree->Branch("b2Pt",&b2Pt,"b2Pt/F");
	tree->Branch("b2Eta",&b2Eta,"b2Eta/F");
	tree->Branch("b2Phi",&b2Phi,"b2Phi/F");

	tree->Branch("WPt",&WPt,"WPt/F");
	tree->Branch("WEta",&WEta,"WEta/F");
	tree->Branch("WPhi",&WPhi,"WPhi/F");

	tree->Branch("ePt",&ePt,"ePt/F");
	tree->Branch("eEta",&eEta,"eEta/F");
	tree->Branch("ePhi",&ePhi,"ePhi/F");

	tree->Branch("muPt",&muPt,"muPt/F");
	tree->Branch("muEta",&muEta,"muEta/F");
	tree->Branch("muPhi",&muPhi,"muPhi/F");

	tree->Branch("vPt",&vPt,"vPt/F");
	tree->Branch("vEta",&vEta,"vEta/F");
	tree->Branch("vPhi",&vPhi,"vPhi/F");

//////////////////////////////////////////////////////////////////////////////

	tree->Branch("ncjets",&ncjets,"ncjets/I");
	tree->Branch("cjetPt",cjetPt,"cjetPt[ncjets]/F");
	tree->Branch("cjetEta",cjetEta,"cjetEta[ncjets]/F");
	tree->Branch("cjetPhi",cjetPhi,"cjetPhi[ncjets]/F");
	tree->Branch("cjetMatched",cjetMatched,"cjetMatched[ncjets]/I");

	tree->Branch("nfwjets",&nfwjets,"nfwjets/I");
	tree->Branch("fwjetPt",fwjetPt,"fwjetPt[nfwjets]/F");
	tree->Branch("fwjetEta",fwjetEta,"fwjetEta[nfwjets]/F");
	tree->Branch("fwjetPhi",fwjetPhi,"fwjetPhi[nfwjets]/F");
	tree->Branch("fwjetMatched",fwjetMatched,"fwjetMatched[nfwjets]/I");

	tree->Branch("ntaujets",&ntaujets,"ntaujets/I");
	tree->Branch("taujetPt",taujetPt,"taujetPt[ntaujets]/F");
	tree->Branch("taujetEta",taujetEta,"taujetEta[ntaujets]/F");
	tree->Branch("taujetPhi",taujetPhi,"taujetPhi[ntaujets]/F");
	tree->Branch("taujetMatched",taujetMatched,"taujetMatched[ntaujets]/I");

	tree->Branch("nisotaujets",&nisotaujets,"nisotaujets/I");
	tree->Branch("isotaujetPt",isotaujetPt,"isotaujetPt[nisotaujets]/F");
	tree->Branch("isotaujetEta",isotaujetEta,"isotaujetEta[nisotaujets]/F");
	tree->Branch("isotaujetPhi",isotaujetPhi,"isotaujetPhi[nisotaujets]/F");
	tree->Branch("isotaujetMatched",isotaujetMatched,"isotaujetMatched[nisotaujets]/I");

	tree->Branch("nmuons",&nmuons,"nmuons/I");
	tree->Branch("muonPt",muonPt,"muonPt[nmuons]/F");
	tree->Branch("muonEta",muonEta,"muonEta[nmuons]/F");
	tree->Branch("muonPhi",muonPhi,"muonPhi[nmuons]/F");

	tree->Branch("negammas",&negammas,"negammas/I");
	tree->Branch("egammaPt",egammaPt,"egammaPt[negammas]/F");
	tree->Branch("egammaEta",egammaEta,"egammaEta[negammas]/F");
	tree->Branch("egammaPhi",egammaPhi,"egammaPhi[negammas]/F");

	tree->Branch("nisoegammas",&nisoegammas,"nisoegammas/I");
	tree->Branch("isoegammaPt",isoegammaPt,"isoegammaPt[nisoegammas]/F");
	tree->Branch("isoegammaEta",isoegammaEta,"isoegammaEta[nisoegammas]/F");
	tree->Branch("isoegammaPhi",isoegammaPhi,"isoegammaPhi[nisoegammas]/F");

	tree->Branch("njets",&njets,"njets/I");
	tree->Branch("jetPt",jetPt,"jetPt[njets]/F");
	tree->Branch("jetEta",jetEta,"jetEta[njets]/F");
	tree->Branch("jetPhi",jetPhi,"jetPhi[njets]/F");
	tree->Branch("jetMatched",jetMatched,"jetMatched[njets]/I");

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

	tree->Branch("njetsGen",&njetsGen,"njetsGen/I");
	tree->Branch("jetPtGen",jetPtGen,"jetPtGen[njetsGen]/F");
	tree->Branch("jetEtaGen",jetEtaGen,"jetEtaGen[njetsGen]/F");
	tree->Branch("jetPhiGen",jetPhiGen,"jetPhiGen[njetsGen]/F");
	tree->Branch("jetGenCMatched",jetGenCMatched,"jetGenCMatched[njetsGen]/I");
	tree->Branch("jetGenFwMatched",jetGenFwMatched,"jetGenFwMatched[njetsGen]/I");

	tree->Branch("metPtGen",&metPtGen,"metPtGen/F");
	tree->Branch("metEtaGen",&metEtaGen,"metEtaGen/F");
	tree->Branch("metPhiGen",&metPhiGen,"metPhiGen/F");

	tree->Branch("hfring0",&hfring0,"hfring0/F");
	tree->Branch("hfring1",&hfring1,"hfring1/F");
	tree->Branch("hfring2",&hfring2,"hfring2/F");
	tree->Branch("hfring3",&hfring3,"hfring3/F");
	tree->Branch("hfring4",&hfring4,"hfring4/F");

	tree->Branch("pu",&pu,"pu/I");


    tree->Branch("npateles",&npateles,"npateles/I");
    tree->Branch("patelePt",patelePt,"patelePt[npateles]/F");
    tree->Branch("pateleEta",pateleEta,"pateleEta[npateles]/F");
    tree->Branch("patelePhi",patelePhi,"patelePhi[npateles]/F");
    tree->Branch("pateleMVAIso",pateleMVAIso,"pateleMVAIso[npateles]/F");

    tree->Branch("npatjets",&npatjets,"npatjets/I");
    tree->Branch("patjetPt",patjetPt,"patjetPt[npatjets]/F");
    tree->Branch("patjetEta",patjetEta,"patjetEta[npatjets]/F");
    tree->Branch("patjetPhi",patjetPhi,"patjetPhi[npatjets]/F");
    tree->Branch("patjetCSV",patjetCSV,"patjetCSV[npatjets]/F");

    tree->Branch("genPVz",&genPVz,"genPVz/F");
    tree->Branch("PVz",&PVz,"PVz/F");
    tree->Branch("offlinePVz",&offlinePVz,"offlinePVz/F");



   //now do what ever initialization is needed

//l1extraL1MuonParticles_hltL1extraParticles__TEST. 722.35 657.6
//l1extraL1EtMissParticles_hltL1extraParticles_MET_TEST. 616.69 534.18
//l1extraL1EtMissParticles_hltL1extraParticles_MHT_TEST. 616.69 533.33
//l1extraL1EmParticles_hltL1extraParticles_Isolated_TEST. 634.52 433.48
//l1extraL1EmParticles_hltL1extraParticles_NonIsolated_TEST. 424.6 391.16
//l1extraL1JetParticles_hltL1extraParticles_Tau_TEST. 525.28 413.77
//l1extraL1JetParticles_hltL1extraParticles_Central_TEST. 452.06 401.72
//l1extraL1JetParticles_hltL1extraParticles_IsoTau_TEST. 451.08 400.31
//l1extraL1JetParticles_hltL1extraParticles_Forward_TEST. 417.84 384.07
//l1extraL1HFRingss_hltL1extraParticles__TEST. 260.46 189.43


}


NtuplerHLTGen::~NtuplerHLTGen()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  -----------


void NtuplerHLTGen::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace reco;
   using namespace std;
   using namespace edm;

  nevent=iEvent.id().event();
  nlumi=iEvent.id().luminosityBlock();
  nrun=iEvent.id().run();
  nfwjets=0;
  ntaujets=0;
  nisotaujets=0;
  ncjets=0;
  njets=0;
  njetsGen=0;
  nmuons=0;
  negammas=0;
  nisoegammas=0;
  pu=-1;

 HPt = -1;
 HEta = 0;
 HPhi = 0;

 b1Pt = -1;
 b1Eta = 0;
 b1Phi = 0;

 b2Pt = -1;
 b2Eta = 0;
 b2Phi = 0;

 WPt = -1;
 WEta = 0;
 WPhi = 0;

 ePt = -1;
 eEta = 0;
 ePhi = 0;

 muPt = -1;
 muEta = 0;
 muPhi = 0;

 vPt = -1;
 vEta = 0;
 vPhi = 0;


 pfmet=-1;
 pfmetPhi=-1;
 pfmht=-1;
 pfmhtPhi=-1; 

 patmet=-1;
 patmetPhi=-1;

 nCSVPF=0;
 npfjets=0;
 nnoelepfjets=0;
 npatjets=0;
 WPtMHT_reco=-1;
 WPt_reco=-1;
 HPt_reco=-1;
 
 genPVz=0;
    
  // Jets: Central, Forward, Tau, IsoTau
  
  edm::Handle< edm::View<reco::Candidate> > jetParticleCentralH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","Central"),jetParticleCentralH);
  const edm::View<reco::Candidate> & jetParticleCentrals = *jetParticleCentralH.product();
  
  edm::Handle< edm::View<reco::Candidate> > jetParticleForwardH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","Forward"),jetParticleForwardH);
  const edm::View<reco::Candidate> & jetParticleForwards = *jetParticleForwardH.product();

  edm::Handle< edm::View<reco::Candidate> > jetParticleTauH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","Tau"),jetParticleTauH);
  const edm::View<reco::Candidate> & jetParticleTaus = *jetParticleTauH.product();

  edm::Handle< edm::View<reco::Candidate> > jetParticleIsoTauH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","IsoTau"),jetParticleIsoTauH);
  const edm::View<reco::Candidate> & jetParticleIsoTaus = *jetParticleIsoTauH.product();

  // Muons

  edm::Handle< edm::View<reco::Candidate> > muonParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles",""),muonParticleH);
  const edm::View<reco::Candidate> & muonParticles = *muonParticleH.product();

  // EG

  edm::Handle< edm::View<reco::Candidate> > egammaParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","NonIsolated"),egammaParticleH);
  const edm::View<reco::Candidate> & egammaParticles = *egammaParticleH.product();

  edm::Handle< edm::View<reco::Candidate> > isoegammaParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","Isolated"),isoegammaParticleH);
  const edm::View<reco::Candidate> & isoegammaParticles = *isoegammaParticleH.product();

  // MET/MHT (ET/HT)

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > metParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","MET"),metParticleH);
  const edm::View<l1extra::L1EtMissParticle> & metParticle = *metParticleH.product();

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > mhtParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles","MHT"),mhtParticleH);
  const edm::View<l1extra::L1EtMissParticle> & mhtParticle = *mhtParticleH.product();

  // HF rings

  edm::Handle< edm::View<l1extra::L1HFRings> > hfringParticleH;
  iEvent.getByLabel(edm::InputTag("hltL1extraParticles",""),hfringParticleH);
  const edm::View<l1extra::L1HFRings> & hfringParticle = *hfringParticleH.product();

  ////////////////// Other info /////////////////////////////////

//  edm::Handle< edm::View<reco::Candidate> > 
  edm::Handle< edm::View<reco::Candidate> > pfMetH;
  iEvent.getByLabel(edm::InputTag("genMetTrue"), pfMetH);
  const edm::View<reco::Candidate> & pfMets = *pfMetH.product();
  //cout<<"pfMet="<<pfMets.begin()->et()<<endl;

  edm::Handle< edm::View<reco::Candidate> > pfJetH;
  iEvent.getByLabel(edm::InputTag("ak5GenJets"), pfJetH);
  const edm::View<reco::Candidate> & pfJets = *pfJetH.product();

//  edm::Handle < std::vector < PileupSummaryInfo  > > puHandle;
//  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puHandle);
//  const PileupSummaryInfo & puObj = *puHandle->begin();
//  if(puHandle->size() < 1) //cout<<"********* Check PileUp stuff !! *******" <<endl;
//  pu = puObj.getTrueNumInteractions();

//  //cout<<"pfJet="<<pfJets.begin()->et()<<endl;


//  edm::Handle< std::vector<reco::PFJet> > pfJets_;
//  iEvent.getByLabel(edm::InputTag("ak4PFJetsCHS"), pfJets_);
//  const edm::View<reco::Candidate> & mhtParticle = *mhtParticleH.product();

//recoPFMETs_pfMet__RECO.
//recoPFJets_ak4PFJetsCHS__RECO.



//kRing1PosEta 	
//kRing1NegEta 	
//kRing2PosEta 	
//kRing2NegEta 	
//kNumRings 
//  l1extra::L1HFRings::HFRingLabels index;
//  index = l1extra::L1HFRings::kRing1PosEta;

	  hfring0=0;
	  hfring1=0;
	  hfring2=0;
	  hfring3=0;
	  hfring4=0;

  if(hfringParticle.size()<=0){
	cout<<endl<<"hfringParticle.size<=0 !!"<<endl;
	}
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


  for( edm::View<reco::Candidate>::const_iterator jetGen=pfJets.begin(); jetGen!=pfJets.end(); jetGen++ )
  {
	if(jetGen->pt()<GenPtThreshold) continue;
  	if(njetsGen<N_jetGen)
  	{  	

	  	jetPtGen[njetsGen]=jetGen->pt();
	  	jetEtaGen[njetsGen]=jetGen->eta();
	  	jetPhiGen[njetsGen]=jetGen->phi();
//	  	jetGenCMatched[njetsGen]=Match(*jetGen,jetParticleCentrals);
//	  	jetGenFwMatched[njetsGen]=Match(*jetGen,jetParticleForwards);
	  	njetsGen++;
	  	  	}
  	else
  	{
  	cout<<endl<<"N_jetGen>=njetsGen !!"<<endl;
  	}
  }

  for(edm::View<reco::Candidate>::const_iterator cjet=jetParticleCentrals.begin(); cjet!=jetParticleCentrals.end(); cjet++ )
  {
  	if(ncjets<N_jet)
  	{  	
	  	cjetPt[ncjets]=cjet->pt();
	  	cjetEta[ncjets]=cjet->eta();
	  	cjetPhi[ncjets]=cjet->phi();
//	  	cjetMatched[ncjets]=Match(*cjet,pfJets);
	  	ncjets++;

	  	jetPt[njets]=cjet->pt();
	  	jetEta[njets]=cjet->eta();
	  	jetPhi[njets]=cjet->phi();
//	  	jetMatched[ncjets]=Match(*cjet,pfJets);
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
//	  	fwjetMatched[ncjets]=Match(*fwjet,pfJets);
	  	nfwjets++;

	  	jetPt[njets]=fwjet->pt();
	  	jetEta[njets]=fwjet->eta();
	  	jetPhi[njets]=fwjet->phi();
//	  	jetMatched[ncjets]=Match(*fwjet,pfJets);
	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=nfwjets !!"<<endl;
  	}
  }

  for(edm::View<reco::Candidate>::const_iterator taujet=jetParticleTaus.begin(); taujet!=jetParticleTaus.end(); taujet++ )
  {
  	if(ntaujets<N_jet)
  	{  	
	  	taujetPt[ntaujets]=taujet->pt();
	  	taujetEta[ntaujets]=taujet->eta();
	  	taujetPhi[ntaujets]=taujet->phi();
//	  	taujetMatched[ncjets]=Match(*taujet,pfJets);
	  	ntaujets++;

//	  	jetPt[njets]=taujet->pt();
//	  	jetEta[njets]=taujet->eta();
//	  	jetPhi[njets]=taujet->phi();
//	  	jetMatched[ncjets]=Match(*taujet,pfJets);
//	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=ntaujets !!"<<endl;
  	}
  }

  for(edm::View<reco::Candidate>::const_iterator isotaujet=jetParticleIsoTaus.begin(); isotaujet!=jetParticleIsoTaus.end(); isotaujet++ )
  {
  	if(nisotaujets<N_jet)
  	{  	
	  	isotaujetPt[nisotaujets]=isotaujet->pt();
	  	isotaujetEta[nisotaujets]=isotaujet->eta();
	  	isotaujetPhi[nisotaujets]=isotaujet->phi();
//	  	isotaujetMatched[ncjets]=Match(*isotaujet,pfJets);
	  	nisotaujets++;

//	  	jetPt[njets]=isotaujet->pt();
//	  	jetEta[njets]=isotaujet->eta();
//	  	jetPhi[njets]=isotaujet->phi();
//	  	jetMatched[ncjets]=Match(*isotaujet,pfJets);
//	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=nisotaujets !!"<<endl;
  	}
  }

  for(edm::View<reco::Candidate>::const_iterator egamma=egammaParticles.begin(); egamma!=egammaParticles.end(); egamma++ )
  {
  	if(negammas<N_jet)
  	{  	
	  	egammaPt[negammas]=egamma->pt();
	  	egammaEta[negammas]=egamma->eta();
	  	egammaPhi[negammas]=egamma->phi();
//	  	egammaMatched[ncjets]=Match(*egamma,pfJets);
	  	negammas++;

//	  	jetPt[njets]=egamma->pt();
//	  	jetEta[njets]=egamma->eta();
//	  	jetPhi[njets]=egamma->phi();
//	  	jetMatched[ncjets]=Match(*egamma,pfJets);
//	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=negammas !!"<<endl;
  	}
  }


  for(edm::View<reco::Candidate>::const_iterator isoegamma=isoegammaParticles.begin(); isoegamma!=isoegammaParticles.end(); isoegamma++ )
  {
  	if(nisoegammas<N_jet)
  	{  	
	  	isoegammaPt[nisoegammas]=isoegamma->pt();
	  	isoegammaEta[nisoegammas]=isoegamma->eta();
	  	isoegammaPhi[nisoegammas]=isoegamma->phi();
//	  	isoegammaMatched[ncjets]=Match(*isoegamma,pfJets);
	  	nisoegammas++;

//	  	jetPt[njets]=isoegamma->pt();
//	  	jetEta[njets]=isoegamma->eta();
//	  	jetPhi[njets]=isoegamma->phi();
//	  	jetMatched[ncjets]=Match(*isoegamma,pfJets);
//	  	njets++;
	  	
  	}
  	else
  	{
  	cout<<endl<<"N_jet>=nisoegammas !!"<<endl;
  	}
  }


/* edm::Handle<vector<reco::GenParticle> > genParticlesH;
 iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesH);
 typedef const reco::CompositeRefCandidateT< GenParticleRefVector >* gPart;
 if(!genParticlesH.failedToGet())
 {
 gPart fake = new reco::CompositeRefCandidateT< GenParticleRefVector >();
 gPart H=fake,W=fake,b1=fake,b2=fake,v=fake ,mu=fake,e=fake, temp=fake;
 const vector<reco::GenParticle> & genParticles = *genParticlesH.product();
 bool b1_found=false;
 bool b2_found=false;
 bool H_found=false;
 bool W_found=false;
 bool e_found=false;
 bool mu_found=false;
 bool v_found=false;
 bool all_found=false;

     for(vector<reco::GenParticle>::const_iterator it = genParticles.begin() ; it != genParticles.end() && !all_found ; it++)
     {
//     if(abs(it->pdgId())==25 ){
//        H_found=true;
//        H = &(*it);
//        //cout<< "."; 
//    }
    if(abs(it->pdgId())==5 && it->mother()->pdgId()==25 ){
        if (!H_found){
            H = (gPart) it->mother();
            H_found=true;
            //cout<< ".";
        } 
        if(!b1_found) {
            b1 = (gPart) &(*it);
            b1_found=true;}
        else if(!b2_found) {
            b2 = (gPart) &(*it);
            b2_found=true;}
        else //cout<<"something wrong:" << endl;
    }
    if(abs(it->pdgId())==11 && abs(it->mother()->pdgId())==24   && it->mother()->mass()>20  && it->mother()->mass()<200 ) {
        if (!W_found){
            W_found=true;
            W = (gPart) it->mother();
            //cout<< "*";
        }
        e_found=true;
        e = &(*(it));
    	
    }
    if(abs(it->pdgId())==13 && abs(it->mother()->pdgId())==24   && it->mother()->mass()>20  && it->mother()->mass()<200 ){
        if (!W_found){
            W_found=true;
            W = (gPart) it->mother();
            //cout<< "*";
        }
        mu_found=true;
        mu = &(*(it));
    }
    if( (abs(it->pdgId())==12 || abs(it->pdgId())==14 || abs(it->pdgId())==16)&& abs(it->mother()->pdgId())==24 && it->mother()->mass()>20  && it->mother()->mass()<200 ){
        if (!W_found){
            W_found=true;
            W = (gPart) it->mother();
            //cout<< "*";
        }
        v_found=true;
        v = &(*(it));
        
    }
    if(H_found && b1_found && b2_found && W_found && (e_found || mu_found) && v_found ) all_found = true;
    }
    if(b2->pt() > b1->pt())
        {
        temp = b1;
        b1 = b2;
        b2 = temp;
        }
    
    //cout<< "H_found "<< H_found << endl; 
    //cout<< "b1_found "<< b1_found << endl; 
    //cout<< "b2_found "<< b2_found << endl; 
    //cout<< "W_found "<< W_found << endl; 
    //cout<< "e_found "<< e_found << endl; 
    //cout<< "v_found "<< v_found << endl; 

//    if(all_found)
    {
        HPt = H->pt();
        HEta = H->eta();
        HPhi = H->phi();

        b1Pt = b1->pt();
        b1Eta = b1->eta();
        b1Phi = b1->phi();

        b2Pt = b2->pt();
        b2Eta = b2->eta();
        b2Phi = b2->phi();

        WPt = W->pt();
        WEta = W->eta();
        WPhi = W->phi();

        ePt = e->pt();
        eEta = e->eta();
        ePhi = e->phi();

        muPt = mu->pt();
        muEta = mu->eta();
        muPhi = mu->phi();

        vPt = v->pt();
        vEta = v->eta();
        vPhi = v->phi();
    }
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
/*

    float pfmet, pfmetPhi, pfmht, pfmhtPhi; 

    int nCSVPF;
    float CSVPF[N_jet], CSVPF_jetPt[N_jet],CSVPF_jetEta[N_jet],CSVPF_jetPhi[N_jet];

    int npfjets;
    float pfjetPt[N_jet],pfjetEta[N_jet],pfjetPhi[N_jet];

    int nnoelepfjets;
    float noelepfjetPt[N_jet],noelepfjetEta[N_jet],noelepfjetPhi[N_jet];

    float WPtMHT_reco, WPt_reco, HPt_reco;
    *isoegammaParticleH.product()


    int neleWP75s;
    float eleWP75Pt[N_jet],eleWP75Eta[N_jet],eleWP75Phi[N_jet];

    int neleWP85s;
    float eleWP85Pt[N_jet],eleWP85Eta[N_jet],eleWP85Phi[N_jet];

    float WPtMHT_reco, WPt_reco, HPt_reco;
    
*/

edm::Handle<edm::View<reco::Candidate> > candidates;
Handle<JetTagCollection> jetTags;
Handle<trigger::TriggerFilterObjectWithRefs> triggerFilterWithRefs;


npfjets=0;
iEvent.getByLabel(edm::InputTag("hltAK4PFJetsCorrected"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    for(const auto& candidate : *candidates.product() ){
        if(npfjets>=N_jet || candidate.pt()<10) continue;
        pfjetPt[npfjets]=candidate.pt();
        pfjetEta[npfjets]=candidate.eta();
        pfjetPhi[npfjets]=candidate.phi();
        npfjets++;
    }
}

nnoelepfjets=0;
iEvent.getByLabel(edm::InputTag("hltJetFilterEle27WP85Barrel"),triggerFilterWithRefs);
if(!triggerFilterWithRefs.failedToGet() && triggerFilterWithRefs.isValid() && (*triggerFilterWithRefs.product()).pfjetRefs().size()>0){
    for(const auto& candidate : (*triggerFilterWithRefs.product()).pfjetRefs() ){
        if(nnoelepfjets>=N_jet) continue;
        noelepfjetPt[nnoelepfjets]=candidate.get()->pt();
        noelepfjetEta[nnoelepfjets]=candidate.get()->eta();
        noelepfjetPhi[nnoelepfjets]=candidate.get()->phi();
        nnoelepfjets++;
    }
}

neleWP75s=0;
iEvent.getByLabel(edm::InputTag("hltEle27WP75NoErGsfTrackIsoFilter"),triggerFilterWithRefs);
if(!triggerFilterWithRefs.failedToGet() && triggerFilterWithRefs.isValid() && (*triggerFilterWithRefs.product()).photonRefs().size()>0){
    for(const auto& candidate : (*triggerFilterWithRefs.product()).photonRefs() ){
        if(neleWP75s>=N_jet) continue;
        eleWP75Pt[neleWP75s]=candidate.get()->pt();
        eleWP75Eta[neleWP75s]=candidate.get()->eta();
        eleWP75Phi[neleWP75s]=candidate.get()->phi();
        neleWP75s++;
    }
}

neleWP85s=0;
iEvent.getByLabel(edm::InputTag("hltEle27WP85BarrelGsfTrackIsoFilter"),triggerFilterWithRefs);
if(!triggerFilterWithRefs.failedToGet() && triggerFilterWithRefs.isValid() && (*triggerFilterWithRefs.product()).photonRefs().size()>0){
    for(const auto& candidate : (*triggerFilterWithRefs.product()).photonRefs() ){
        if(neleWP85s>=N_jet) continue;
        eleWP85Pt[neleWP85s]=candidate.get()->pt();
        eleWP85Eta[neleWP85s]=candidate.get()->eta();
        eleWP85Phi[neleWP85s]=candidate.get()->phi();
        neleWP85s++;
    }
}

neleWPTights=0;
iEvent.getByLabel(edm::InputTag("hltEle32WPTightGsfTrackIsoFilter"),triggerFilterWithRefs);
if(!triggerFilterWithRefs.failedToGet() && triggerFilterWithRefs.isValid() && (*triggerFilterWithRefs.product()).photonRefs().size()>0){
    for(const auto& candidate : (*triggerFilterWithRefs.product()).photonRefs() ){
        if(neleWPTights>=N_jet) continue;
        eleWPTightPt[neleWPTights]=candidate.get()->pt();
        eleWPTightEta[neleWPTights]=candidate.get()->eta();
        eleWPTightPhi[neleWPTights]=candidate.get()->phi();
        neleWPTights++;
    }
}

neleWPLooses=0;
iEvent.getByLabel(edm::InputTag("hltEle32WPLooseGsfTrackIsoFilter"),triggerFilterWithRefs);
if(!triggerFilterWithRefs.failedToGet() && triggerFilterWithRefs.isValid() && (*triggerFilterWithRefs.product()).photonRefs().size()>0){
    for(const auto& candidate : (*triggerFilterWithRefs.product()).photonRefs() ){
        if(neleWPLooses>=N_jet) continue;
        eleWPLoosePt[neleWPLooses]=candidate.get()->pt();
        eleWPLooseEta[neleWPLooses]=candidate.get()->eta();
        eleWPLoosePhi[neleWPLooses]=candidate.get()->phi();
        neleWPLooses++;
    }
}


iEvent.getByLabel(edm::InputTag("hltPFMETProducer"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    pfmet = candidates->begin()->pt();
    pfmetPhi = candidates->begin()->phi();
}
//cout<<"e\n";

iEvent.getByLabel(edm::InputTag("hltPFMHTLooseID"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    pfmht = candidates->begin()->pt();
    pfmhtPhi = candidates->begin()->phi();
}
iEvent.getByLabel(edm::InputTag("genSelectorH"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    HPt =  candidates->begin()->pt();
    HEta =  candidates->begin()->eta();
    HPhi =  candidates->begin()->phi();
    genPVz =  candidates->begin()->vertex().z();
}
iEvent.getByLabel(edm::InputTag("genSelectorBs"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    b1Pt =  candidates->begin()->pt();
    b1Eta =  candidates->begin()->eta();
    b1Phi =  candidates->begin()->phi();
    b2Pt =  candidates->at(1).pt();
    b2Eta =  candidates->at(1).eta();
    b2Phi =  candidates->at(1).phi();
}

//cout<<"b\n";
iEvent.getByLabel(edm::InputTag("genSelectorW"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    WPt =  candidates->begin()->pt();
    WEta =  candidates->begin()->eta();
    WPhi =  candidates->begin()->phi();
}
//cout<<"e\n";
iEvent.getByLabel(edm::InputTag("genSelectorE"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    ePt =  candidates->begin()->pt();
    eEta =  candidates->begin()->eta();
    ePhi =  candidates->begin()->phi();
}
//cout<<"a\n";
iEvent.getByLabel(edm::InputTag("genSelectorMu"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    muPt =  candidates->begin()->pt();
    muEta =  candidates->begin()->eta();
    muPhi =  candidates->begin()->phi();
}
//cout<<"e\n";

iEvent.getByLabel(edm::InputTag("genSelectorNu"),candidates);
if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
    vPt =  candidates->begin()->pt();
    vEta =  candidates->begin()->eta();
    vPhi =  candidates->begin()->phi();
}
//triggerTriggerFilterObjectWithRefs_hltL1EGHttEle20WP85GsfTrackIsoFilter__TEST.obj.photonRefs().get().pt()
nCSVPF=0;
iEvent.getByLabel("hltCombinedSecondaryVertexBJetTagsPF",jetTags);
if(!jetTags.failedToGet() && jetTags.isValid() && jetTags->size()>0){
    for(const auto& candidate : *jetTags.product() ){
        if(nCSVPF>=N_jet) continue;
        CSVPF[nCSVPF]=candidate.second;
        CSVPF_jetPt[nCSVPF]=candidate.first->pt();
        CSVPF_jetEta[nCSVPF]=candidate.first->eta();
        CSVPF_jetPhi[nCSVPF]=candidate.first->phi();
        nCSVPF++;
    }
}

//|| (Sum$(l1extraL1EmParticles_hltL1extraParticles_Isolated_TEST.obj.pt()>=30 && abs(l1extraL1EmParticles_hltL1extraParticles_Isolated_TEST.obj.eta())<=2.2)>=1))");
//    preselection = "(Sum$(recoCandidatesOwned_genSelectorE__TEST.obj.data_.pt()>20 && abs(recoCandidatesOwned_genSelectorE__TEST.obj.data_.eta())<2.4)>=1) && (Sum$(recoCandidatesOwned_genSelectorBs__TEST.obj.data_.pt()>20 && abs(recoCandidatesOwned_genSelectorBs__TEST.obj.data_.eta())<2.4)>=2) && (Sum$(recoCandidatesOwned_genSelectorH__TEST.obj.data_.pt()>100)>=1)";


//HPt_reco
float tmp=0;
if(nnoelepfjets>0) HPt_reco = noelepfjetPt[0];
for(int i=0; i<nnoelepfjets; i++ ){
    for(int j=0; j<nnoelepfjets; j++ ){
        if(i!=j){
            tmp = sumpt(noelepfjetPt[i],noelepfjetPhi[i],noelepfjetPt[j],noelepfjetPhi[j]);
            if(tmp>HPt_reco) HPt_reco = tmp;
        }
    }
}

tmp=0;
if(neleWP85s>0)
{
    WPt_reco = sumpt(pfmet,pfmetPhi,eleWP85Pt[0],eleWP85Phi[0]);
    WPtMHT_reco = sumpt(pfmht,pfmhtPhi,eleWP85Pt[0],eleWP85Phi[0]);
} else if(neleWP75s>0){
    WPt_reco = sumpt(pfmet,pfmetPhi,eleWP75Pt[0],eleWP75Phi[0]);
    WPtMHT_reco = sumpt(pfmht,pfmhtPhi,eleWP75Pt[0],eleWP75Phi[0]);
} else if(neleWPLooses>0){
    WPt_reco = sumpt(pfmet,pfmetPhi,eleWPLoosePt[0],eleWPLoosePhi[0]);
    WPtMHT_reco = sumpt(pfmht,pfmhtPhi,eleWPLoosePt[0],eleWPLoosePhi[0]);
} else if(neleWPTights>0){
    WPt_reco = sumpt(pfmet,pfmetPhi,eleWPTightPt[0],eleWPTightPhi[0]);
    WPtMHT_reco = sumpt(pfmht,pfmhtPhi,eleWPTightPt[0],eleWPTightPhi[0]);
}



Handle< edm::View< pat::Electron> > pateles;
npateles=0;
iEvent.getByLabel(edm::InputTag("slimmedElectrons"),pateles);
if(!pateles.failedToGet() && pateles.isValid() && (*pateles.product()).size()>0){
    for(const auto& candidate : (*pateles.product()) ){
        if(npateles>=N_jet) continue;
        if(candidate.pt()<5) continue;
        patelePt[npateles]=candidate.pt();
        pateleEta[npateles]=candidate.eta();
        patelePhi[npateles]=candidate.phi();
        pateleMVAIso[npateles]=candidate.mva_Isolated();
        npateles++;
    }
}

Handle< edm::View< pat::Jet> > patjets;
npatjets=0;
iEvent.getByLabel(edm::InputTag("slimmedJets"),patjets);
if(!patjets.failedToGet() && patjets.isValid() && (*patjets.product()).size()>0){
    for(const auto& candidate : (*patjets.product()) ){
        if(npatjets>=N_jet) continue;
        if(candidate.pt()<10) continue;
        patjetPt[npatjets]=candidate.pt();
        patjetEta[npatjets]=candidate.eta();
        patjetPhi[npatjets]=candidate.phi();
        patjetCSV[npatjets]=candidate.bDiscriminator("combinedInclusiveSecondaryVertexV2BJetTags");
        npatjets++;
    }
}

Handle< edm::View< pat::MET> > patMETs;
iEvent.getByLabel(edm::InputTag("slimmedMETs"),patMETs);



Handle< edm::View< reco::Vertex> > vertexs;
offlinePVz=0;
iEvent.getByLabel(edm::InputTag("offlineSlimmedPrimaryVertices"),vertexs);
if(!vertexs.failedToGet() && vertexs.isValid() && (*vertexs.product()).size()>0){
    offlinePVz=(*vertexs.product()).begin()->z();
}

PVz=0;
iEvent.getByLabel(edm::InputTag("hltVerticesPF"),vertexs);
if(!vertexs.failedToGet() && vertexs.isValid() && (*vertexs.product()).size()>0){
    PVz=(*vertexs.product()).begin()->z();
}


//Handle< edm::View< SimVertex> > simvertexs;
//genPVz=0;
//iEvent.getByLabel(edm::InputTag("g4SimHits"),simvertexs);
//if(!simvertexs.failedToGet() && simvertexs.isValid() && (*simvertexs.product()).size()>2){
//    genPVz=(*simvertexs.product()).at(2).position().z();
//}

    
//iEvent.getByLabel(edm::InputTag("hltPFMETProducer"),candidates);
//if(!candidates.failedToGet() && candidates.isValid() && candidates->size()>0){
//    pfmet = candidates->begin()->pt();
//    pfmetPhi = candidates->begin()->phi();
//}


//  PtSorter(jetPt,jetEta,jetPhi,njets);
  tree->Fill();

//cout<<"g\n";
return;
//cout<<"e\n";

}


// ------------ method called once each job just before starting event loop  ------------
void 
NtuplerHLTGen::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
NtuplerHLTGen::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
NtuplerHLTGen::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
NtuplerHLTGen::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
NtuplerHLTGen::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
NtuplerHLTGen::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
NtuplerHLTGen::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerHLTGen);
