// -*- C++ -*-
//
// Package:    RemovePileUpDominatedEventsGen/RemovePileUpDominatedEventsGen
// Class:      RemovePileUpDominatedEventsGen
// 
/**\class RemovePileUpDominatedEventsGen RemovePileUpDominatedEventsGen.cc RemovePileUpDominatedEventsGen/RemovePileUpDominatedEventsGen/plugins/RemovePileUpDominatedEventsGen.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Silvio DONATO
//         Created:  Fri, 12 Dec 2014 12:48:57 GMT
//
//

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>
#include <SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h>

class RemovePileUpDominatedEventsGen : public edm::EDFilter {
   public:
      explicit RemovePileUpDominatedEventsGen(const edm::ParameterSet&);
      ~RemovePileUpDominatedEventsGen();
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryInfos_;
      const edm::EDGetTokenT<GenEventInfoProduct > generatorInfo_;
      unsigned int bunchCrossing;
};

RemovePileUpDominatedEventsGen::RemovePileUpDominatedEventsGen(const edm::ParameterSet& iConfig):
pileupSummaryInfos_(    consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileupSummaryInfos"))    ),
generatorInfo_(         consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generatorInfo"))                     )
{
bunchCrossing=0;
produces<float>();
}

RemovePileUpDominatedEventsGen::~RemovePileUpDominatedEventsGen() {}

bool RemovePileUpDominatedEventsGen::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   using namespace edm;
   using namespace std;
   
   edm::Handle<GenEventInfoProduct > generatorInfo;
   iEvent.getByToken(generatorInfo_,generatorInfo);

   edm::Handle<std::vector<PileupSummaryInfo> > pileupSummaryInfos;
   iEvent.getByToken(pileupSummaryInfos_,pileupSummaryInfos);
   
   //find in-time pile-up
   if(bunchCrossing>=pileupSummaryInfos.product()->size() || pileupSummaryInfos.product()->at(bunchCrossing).getBunchCrossing()!=0)
   {
       bool found=false;
       for(bunchCrossing=0; bunchCrossing<pileupSummaryInfos.product()->size() && !found; bunchCrossing++)
       {
           if( pileupSummaryInfos.product()->at(bunchCrossing).getBunchCrossing() == 0 ) found=true;
       }
       if(!found){
            edm::LogInfo("RemovePileUpDominatedEventsGen") << "In-time pile-up not found!" << endl;
            return true;
       }
   }

   //get the PU pt-hat max
   float signal_pT_hat = -1;
   float pu_pT_hat_max = -1;
   
   PileupSummaryInfo puSummary_onTime = pileupSummaryInfos.product()->at(bunchCrossing);
   for(const auto& pu_pT_hat : puSummary_onTime.getPU_pT_hats()) if (pu_pT_hat>pu_pT_hat_max) pu_pT_hat_max = pu_pT_hat;

   //get the signal pt-hat
   signal_pT_hat = generatorInfo->qScale();

   //save PU - signal pt-hat
   std::auto_ptr<float> pOut(new float());
   *pOut=signal_pT_hat-pu_pT_hat_max;   
   iEvent.put(pOut);

   //filter the event
   if (signal_pT_hat>pu_pT_hat_max) return true;
   return false;
}

void RemovePileUpDominatedEventsGen::beginJob(){
}
void RemovePileUpDominatedEventsGen::endJob() {}

void RemovePileUpDominatedEventsGen::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag> ("pileupSummaryInfos",edm::InputTag("addPileupInfo"));
  desc.add<edm::InputTag> ("generatorInfo",edm::InputTag("generator"));
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemovePileUpDominatedEventsGen);
