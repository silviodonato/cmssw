// -*- C++ -*-
//
// Package:    GenParticleFilter
// Class:      GenParticleFilter
// 
/**\class GenParticleFilter GenParticleFilter.cc GenParticleFilter/GenParticleFilter/src/GenParticleFilter.cc

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
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class GenParticleFilter : public edm::EDFilter {
   public:
      explicit GenParticleFilter(const edm::ParameterSet&);
      ~GenParticleFilter();

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
GenParticleFilter::GenParticleFilter(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


GenParticleFilter::~GenParticleFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
GenParticleFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;



  edm::Handle<vector<reco::GenParticle> > genParticlesH;
  iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesH);
  const vector<reco::GenParticle> & genParticles = *genParticlesH.product();

  int count=0;
  for(size_t i=0;i<(genParticles).size();i++)
		{
		   if(std::fabs(genParticles[i].pdgId())==5)
		   	if(std::fabs(genParticles[i].mother()->pdgId())==25)
		   		if(fabs(genParticles[i].eta())<2.4)
			   		count++;
		}
if(count<2)		
{   return false;}

return true;
   
}

// ------------ method called once each job just before starting event loop  ------------
void 
GenParticleFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
GenParticleFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
GenParticleFilter::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
GenParticleFilter::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
GenParticleFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
GenParticleFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenParticleFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(GenParticleFilter);
