#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiSoA_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiSoA_h

#include <cstdint>

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"

namespace hgcaldigi {

  // Lossless storage for legacy HGCDataFrame<DetId, HGCSample> with at most
  // five samples. The 12 data bits and three status bits are packed per sample;
  // bits 12-28 are stored once when shared by all samples. Rare exceptions
  // are kept as full words in a parallel product indexed by exceptionIndex.
  // DetIds are stored as 16-bit deltas with sparse absolute-ID exceptions.
  GENERATE_SOA_LAYOUT(HGCalLegacyDigiSoALayout,
                      SOA_COLUMN(uint16_t, detIdDelta),
                      SOA_COLUMN(uint8_t, sampleCount),
                      SOA_COLUMN(uint32_t, sharedBits),
                      SOA_COLUMN(uint16_t, statusBits),
                      SOA_COLUMN(uint64_t, dataBits),
                      SOA_COLUMN(uint32_t, exceptionIndex))
  using HGCalLegacyDigiSoA = HGCalLegacyDigiSoALayout<>;

}  // namespace hgcaldigi

#endif
