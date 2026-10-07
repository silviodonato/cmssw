#ifndef DataFormats_HGCalDigi_interface_HGCalDigiCompressedHost_h
#define DataFormats_HGCalDigi_interface_HGCalDigiCompressedHost_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCompressedSoA.h"

namespace hgcaldigi {

  using HGCalDigiCompressedHost = PortableHostCollection<HGCalDigiCompressedSoA>;

}  // namespace hgcaldigi

#endif  // DataFormats_HGCalDigi_interface_HGCalDigiCompressedHost_h
