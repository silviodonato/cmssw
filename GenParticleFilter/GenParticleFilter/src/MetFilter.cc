// -*- C++ -*-
//
// Package:    MetFilter
// Class:      MetFilter
// 
/**\class MetFilter MetFilter.cc MetFilter/MetFilter/src/MetFilter.cc

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
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/MET.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class MetFilter : public edm::EDFilter {
   public:
      explicit MetFilter(const edm::ParameterSet&);
      ~MetFilter();

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
MetFilter::MetFilter(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


MetFilter::~MetFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
MetFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;



  edm::Handle<edm::View<reco::MET> >  GenMETsH;
  iEvent.getByLabel(edm::InputTag("genMetTrue"),GenMETsH);
//  const vector<edm::View<reco::MET> > & GenMETs = *GenMETsH.product();
if(GenMETsH->size()==0)
{
 cout<<"No MET present!";
 return false;
}
  edm::View<reco::MET>::const_iterator MET=GenMETsH->begin();
  if(  (MET->pt()>80) )	return true;
  else 	return false;
   
}

// ------------ method called once each job just before starting event loop  ------------
void 
MetFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MetFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
MetFilter::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
MetFilter::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
MetFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
MetFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MetFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(MetFilter);
