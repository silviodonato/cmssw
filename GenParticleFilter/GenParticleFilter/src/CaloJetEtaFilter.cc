// -*- C++ -*-
//
// Package:    CaloJetEtaFilter
// Class:      CaloJetEtaFilter
// 
/**\class CaloJetEtaFilter CaloJetEtaFilter.cc CaloJetEtaFilter/CaloJetEtaFilter/src/CaloJetEtaFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Silvio DONATO
//         Created:  Thu Nov  7 10:52:52 CET 2013
// $Id$
//
//


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "DataFormats/JetReco/interface/CaloJet.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

//
// class declaration
//

class CaloJetEtaFilter : public edm::EDFilter {
   public:
      explicit CaloJetEtaFilter(const edm::ParameterSet&);
      ~CaloJetEtaFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual bool beginRun(edm::Run&, edm::EventSetup const&);
      virtual bool endRun(edm::Run&, edm::EventSetup const&);
      virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------

        edm::InputTag m_jet;
      double m_maxeta;

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
CaloJetEtaFilter::CaloJetEtaFilter(const edm::ParameterSet& iConfig)
{
   using namespace edm;
   using namespace std;

   //now do what ever initialization is needed
  produces<std::vector<reco::CaloJet> >(); 
  m_jet = iConfig.getParameter<edm::InputTag>("jet");	
  m_maxeta = iConfig.getParameter<double>("maxeta");
  
}


CaloJetEtaFilter::~CaloJetEtaFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
CaloJetEtaFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;


   std::auto_ptr<std::vector<reco::CaloJet> > pOut(new std::vector<reco::CaloJet> );


  edm::Handle<vector<reco::CaloJet> > caloJetsH;
  iEvent.getByLabel(m_jet,caloJetsH);
//  const vector<reco::CaloJet> & caloJets = *caloJetsH.product();


//  std::vector<reco::TrackIPTagInfo>::const_iterator taginfo = trackIPTagInfos->begin();
//   for( ; taginfo != trackIPTagInfos->end() && it != jetTracksAssociation->end() && i < m_maxNjets; taginfo++, i++, it++ ) {

  std::vector<reco::CaloJet>::const_iterator it = caloJetsH->begin();
//  for(size_t i=0;i<(caloJets).size();i++)
  for(; it!=caloJetsH->end();it++)
		{
		   if(std::fabs(it->eta())<=m_maxeta)
			pOut->push_back(* dynamic_cast<const reco::CaloJet *>(&(*it))); // fill it
//	   		count++;
		}
//if(count<2)		
//{   return false;}

iEvent.put(pOut);
return true;
   
}

// ------------ method called once each job just before starting event loop  ------------
void 
CaloJetEtaFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CaloJetEtaFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
CaloJetEtaFilter::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
CaloJetEtaFilter::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
CaloJetEtaFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
CaloJetEtaFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
CaloJetEtaFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(CaloJetEtaFilter);
