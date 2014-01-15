// -*- C++ -*-
//
// Package:    GenParticleFilter4b
// Class:      GenParticleFilter4b
// 
/**\class GenParticleFilter4b GenParticleFilter4b.cc GenParticleFilter4b/GenParticleFilter4b/src/GenParticleFilter4b.cc

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

class GenParticleFilter4b : public edm::EDFilter {
   public:
      explicit GenParticleFilter4b(const edm::ParameterSet&);
      ~GenParticleFilter4b();

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
GenParticleFilter4b::GenParticleFilter4b(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


GenParticleFilter4b::~GenParticleFilter4b()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
GenParticleFilter4b::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
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
if(count<4)		
{   return false;}

return true;
   
}

// ------------ method called once each job just before starting event loop  ------------
void 
GenParticleFilter4b::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
GenParticleFilter4b::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
GenParticleFilter4b::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
GenParticleFilter4b::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
GenParticleFilter4b::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
GenParticleFilter4b::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenParticleFilter4b::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(GenParticleFilter4b);
