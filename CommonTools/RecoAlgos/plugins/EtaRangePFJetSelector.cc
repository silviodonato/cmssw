/* \class EtaRangePFJetSelector
 *
 * selects PF-jet in the eta range
 *
 * \author: Silvio Donato
 *
 */
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/EtaRangeSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "DataFormats/JetReco/interface/PFJet.h"

 typedef SingleObjectSelector<
           reco::PFJetCollection, 
           EtaRangeSelector
         > EtaRangePFJetSelector;

DEFINE_FWK_MODULE( EtaRangePFJetSelector );
