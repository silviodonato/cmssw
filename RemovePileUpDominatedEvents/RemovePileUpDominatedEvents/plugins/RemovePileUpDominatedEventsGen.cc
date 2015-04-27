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
      const unsigned int bunchCrossing_;
};

RemovePileUpDominatedEventsGen::RemovePileUpDominatedEventsGen(const edm::ParameterSet& iConfig):
pileupSummaryInfos_(    consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileupSummaryInfos"))    ),
generatorInfo_(         consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generatorInfo"))                     ),
bunchCrossing_(         iConfig.getParameter<int>("bunchCrossing")                                                              )
{
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
   
   if(bunchCrossing_>=pileupSummaryInfos.product()->size()){
        edm::LogInfo("RemovePileUpDominatedEventsGen") << "Number of bunch crossing out of size!" << endl;
        return true;
    }

   PileupSummaryInfo puSummary_onTime = pileupSummaryInfos.product()->at(bunchCrossing_);
   if(puSummary_onTime.getBunchCrossing()!=0) edm::LogInfo("RemovePileUpDominatedEventsGen") << "You are considering the pile-up bunch crossing " << puSummary_onTime.getBunchCrossing() << " please fix it." << endl;
   
   float signal_pT_hat = -1;
   float pu_pT_hat_max = -1;
   
   for(const auto& pu_pT_hat : puSummary_onTime.getPU_pT_hats()) if (pu_pT_hat>pu_pT_hat_max) pu_pT_hat_max = pu_pT_hat;



   signal_pT_hat = generatorInfo->qScale();
   std::auto_ptr<float> pOut(new float());
   *pOut=signal_pT_hat-pu_pT_hat_max;   
   iEvent.put(pOut);
   if (signal_pT_hat>pu_pT_hat_max) return true;
   return false;
}

void RemovePileUpDominatedEventsGen::beginJob(){}
void RemovePileUpDominatedEventsGen::endJob() {}

void RemovePileUpDominatedEventsGen::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag> ("pileupSummaryInfos",edm::InputTag("addPileupInfo"));
  desc.add<edm::InputTag> ("generatorInfo",edm::InputTag("generator"));
  desc.add<int> ("bunchCrossing",12); //12 correspond to in-time pile-up
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RemovePileUpDominatedEventsGen);
