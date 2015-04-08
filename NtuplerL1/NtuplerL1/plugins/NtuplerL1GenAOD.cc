// -*- C++ -*-
//
// Package:    NtuplerL1GenAOD/NtuplerL1GenAOD
// Class:      NtuplerL1GenAOD
// 
/**\class NtuplerL1GenAOD NtuplerL1GenAOD.cc NtuplerL1GenAOD/NtuplerL1GenAOD/plugins/NtuplerL1GenAOD.cc

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

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"

#include "DataFormats/L1Trigger/interface/L1HFRings.h"

#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"

#include "DataFormats/Math/interface/deltaR.h"

//#include "NtuplerL1/NtuplerL1/interface/Match.h"
//#include "NtuplerL1/NtuplerL1/interface/PtSorter.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"



//************************************************************************
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "TTree.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#define N_jet       10  
#define N_muon      10  
#define N_jetGen       1000  
//************************************************************************


//
// class declaration
//

class NtuplerL1GenAOD : public edm::EDAnalyzer {
   public:
      explicit NtuplerL1GenAOD(const edm::ParameterSet&);
      ~NtuplerL1GenAOD();

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
      
      int nevent,nlumi,nrun;

      int ncjets,cjetMatched[N_jet];
      float cjetPt[N_jet],cjetEta[N_jet],cjetPhi[N_jet];

      int nfwjets,fwjetMatched[N_jet];
      float fwjetPt[N_jet],fwjetEta[N_jet],fwjetPhi[N_jet];

      int ntaujets,taujetMatched[N_jet];
      float taujetPt[N_jet],taujetEta[N_jet],taujetPhi[N_jet];

      int njets,jetMatched[2*N_jet];
      float jetPt[2*N_jet],jetEta[2*N_jet],jetPhi[2*N_jet];

      int nmuons;
      float muonPt[2*N_muon],muonEta[2*N_muon],muonPhi[2*N_muon];

      int njetsGen,jetGenCMatched[N_jetGen],jetGenFwMatched[N_jetGen];
      float jetPtGen[N_jetGen],jetEtaGen[N_jetGen],jetPhiGen[N_jetGen];

      float metPt,etTot,etMiss,metEta,metPhi;

      float mhtPt,htTot,htMiss,mhtEta,mhtPhi;

      float metPtGen,metEtaGen,metPhiGen;

      int pu;
	
      float hfring1, hfring2, hfring3, hfring4, hfring0;

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
NtuplerL1GenAOD::NtuplerL1GenAOD(const edm::ParameterSet& iConfig)

{

	tree=file->make<TTree>("tree","tree");
	tree->Branch("nevent",&nevent,"nevent/I");
	tree->Branch("nlumi",&nlumi,"nlumi/I");
	tree->Branch("nrun",&nrun,"nrun/I");

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

	tree->Branch("nmuons",&nmuons,"nmuons/I");
	tree->Branch("muonPt",muonPt,"muonPt[nmuons]/F");
	tree->Branch("muonEta",muonEta,"muonEta[nmuons]/F");
	tree->Branch("muonPhi",muonPhi,"muonPhi[nmuons]/F");


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

   //now do what ever initialization is needed

}


NtuplerL1GenAOD::~NtuplerL1GenAOD()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  -----------


void NtuplerL1GenAOD::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace reco;
   using namespace std;
   using namespace edm;

  nevent=iEvent.id().event();
  nlumi=iEvent.id().luminosityBlock();
  nrun=iEvent.id().run();
  nfwjets=0;
  ntaujets=0;
  ncjets=0;
  njets=0;
  njetsGen=0;
  nmuons=0;
  pu=-1;

  edm::Handle< edm::View<reco::Candidate> > jetParticleCentralH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles","Central"),jetParticleCentralH);
  const edm::View<reco::Candidate> & jetParticleCentrals = *jetParticleCentralH.product();
  
  edm::Handle< edm::View<reco::Candidate> > jetParticleForwardH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles","Forward"),jetParticleForwardH);
  const edm::View<reco::Candidate> & jetParticleForwards = *jetParticleForwardH.product();

  edm::Handle< edm::View<reco::Candidate> > muonParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles",""),muonParticleH);
  const edm::View<reco::Candidate> & muonParticles = *muonParticleH.product();

//  edm::Handle< edm::View<reco::Candidate> > jetParticleTauH;
//  iEvent.getByLabel(edm::InputTag("l1extraParticles","Tau"),jetParticleTauH);
//  const edm::View<reco::Candidate> & jetParticleTaus = *jetParticleTauH.product();

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > metParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles","MET"),metParticleH);
  const edm::View<l1extra::L1EtMissParticle> & metParticle = *metParticleH.product();

  edm::Handle< edm::View<l1extra::L1EtMissParticle> > mhtParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles","MHT"),mhtParticleH);
  const edm::View<l1extra::L1EtMissParticle> & mhtParticle = *mhtParticleH.product();

//  edm::Handle< edm::View<reco::Candidate> > 
  edm::Handle< edm::View<reco::Candidate> > pfMetH;
  iEvent.getByLabel(edm::InputTag("genMetTrue"), pfMetH);
  const edm::View<reco::Candidate> & pfMets = *pfMetH.product();
//  cout<<"pfMet="<<pfMets.begin()->et()<<endl;

  edm::Handle< edm::View<reco::Candidate> > pfJetH;
  iEvent.getByLabel(edm::InputTag("ak5GenJets"), pfJetH);
  const edm::View<reco::Candidate> & pfJets = *pfJetH.product();

  edm::Handle < std::vector < PileupSummaryInfo  > > puHandle;
  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puHandle);
  const PileupSummaryInfo & puObj = *puHandle->begin();
  if(puHandle->size() > 1) cout<<"********* Check PileUp stuff !! *******" <<endl;
  pu = puObj.getTrueNumInteractions();

//  cout<<"pfJet="<<pfJets.begin()->et()<<endl;


//  edm::Handle< std::vector<reco::PFJet> > pfJets_;
//  iEvent.getByLabel(edm::InputTag("ak4PFJetsCHS"), pfJets_);
//  const edm::View<reco::Candidate> & mhtParticle = *mhtParticleH.product();

//recoPFMETs_pfMet__RECO.
//recoPFJets_ak4PFJetsCHS__RECO.


  edm::Handle< edm::View<l1extra::L1HFRings> > hfringParticleH;
  iEvent.getByLabel(edm::InputTag("l1extraParticles",""),hfringParticleH);
  const edm::View<l1extra::L1HFRings> & hfringParticle = *hfringParticleH.product();

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
  



  for( edm::View<reco::Candidate>::const_iterator jetGen=pfJets.begin(); jetGen!=pfJets.end(); jetGen++ )
  {
	float GenPtThreshold = 30;
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

//  for(edm::View<reco::Candidate>::const_iterator taujet=jetParticleTaus.begin(); taujet!=jetParticleTaus.end(); taujet++ )
//  {
//  	if(ntaujets<N_jet)
//  	{  	
//	  	taujetPt[ntaujets]=taujet->pt();
//	  	taujetEta[ntaujets]=taujet->eta();
//	  	taujetPhi[ntaujets]=taujet->phi();
//	  	taujetMatched[ncjets]=Match(*taujet,pfJets);
//	  	ntaujets++;

//	  	jetPt[njets]=taujet->pt();
//	  	jetEta[njets]=taujet->eta();
//	  	jetPhi[njets]=taujet->phi();
//	  	jetMatched[ncjets]=Match(*taujet,pfJets);
//	  	njets++;
//	  	
//  	}
//  	else
//  	{
//  	cout<<endl<<"N_jet>=ntaujets !!"<<endl;
//  	}


//  }

//  PtSorter(jetPt,jetEta,jetPhi,njets);
  tree->Fill();

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
NtuplerL1GenAOD::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
NtuplerL1GenAOD::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
NtuplerL1GenAOD::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
NtuplerL1GenAOD::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
NtuplerL1GenAOD::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
NtuplerL1GenAOD::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
NtuplerL1GenAOD::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerL1GenAOD);
