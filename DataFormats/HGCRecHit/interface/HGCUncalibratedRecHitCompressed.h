#ifndef DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED
#define DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED

#include <cstdint>
#include <vector>
#include "DataFormats/DetId/interface/DetId.h"

class HGCUncalibratedRecHit;
class HGCUncalibratedRecHitCompressed {
public:
  typedef DetId key_type;
  using index_type = uint8_t;

  enum Flags {
    kGood = -1,  // channel is good (mutually exclusive with other states)  setFlagBit(kGood) reset flags_ to zero
    kPoorReco,   // channel has been badly reconstructed (e.g. bad shape, bad chi2 etc.)
    kSaturated,  // saturated channel
    kOutOfTime   // channel out of time
  };

  HGCUncalibratedRecHitCompressed();
  HGCUncalibratedRecHitCompressed(const HGCUncalibratedRecHit& hit, index_type geometryIndex);

  // A padding hit advances the geometry-index delta stream and is discarded
  // by the decompressor. Its payload uses the common/default values and its
  // amplitude is deliberately zero.
  static HGCUncalibratedRecHitCompressed makeIndexPadding(index_type geometryIndex) {
    HGCUncalibratedRecHitCompressed padding;
    padding.pedestal_ = -1.f;
    padding.jitter_ = -99.f;
    padding.chi2_ = -1.f;
    padding.OOTchi2_ = 10000.f;
    padding.id_ = geometryIndex;
    return padding;
  }

  virtual ~HGCUncalibratedRecHitCompressed();
  float amplitude() const { return amplitude_; }
  float pedestal() const { return pedestal_; }
  float jitter() const { return jitter_; }
  float chi2() const { return chi2_; }
  float outOfTimeEnergy() const { return OOTamplitude_; }
  float outOfTimeChi2() const { return OOTchi2_; }

  uint32_t flags() const { return flags_; }
  uint32_t aux() const { return aux_; }
  float jitterError() const;
  uint8_t jitterErrorBits() const;
  DetId id() const { return DetId(id_); }

private:
  float amplitude_;     //< Reconstructed amplitude
  float pedestal_;      //< Reconstructed pedestal
  float jitter_;        //< Reconstructed time jitter
  float chi2_;          //< Chi2 of the pulse
  float OOTamplitude_;  //< Out-Of-Time reconstructed amplitude
  float OOTchi2_;       //< Out-Of-Time Chi2
  uint32_t flags_;      //< flag to be propagated to RecHit
  uint32_t aux_;        //< aux word; first 8 bits contain time (jitter) error
  index_type id_;       //< Index in the valid-DetId list, or its delta
};

#endif
