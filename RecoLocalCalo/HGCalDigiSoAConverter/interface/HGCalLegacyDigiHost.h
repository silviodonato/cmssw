#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiHost_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiHost_h

#include "RecoLocalCalo/HGCalDigiSoAConverter/interface/HGCalLegacyDigiSoA.h"
#include "DataFormats/Portable/interface/PortableHostCollection.h"

namespace hgcaldigi {
  using HGCalLegacyDigiHost = PortableHostCollection<HGCalLegacyDigiSoA>;
}  // namespace hgcaldigi

#endif
