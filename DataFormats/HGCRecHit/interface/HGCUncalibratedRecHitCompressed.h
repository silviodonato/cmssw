#ifndef DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED
#define DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED

#include <vector>
#include "DataFormats/DetId/interface/DetId.h"

class HGCUncalibratedRecHit;
class HGCUncalibratedRecHitCompressedsSorted;

class HGCUncalibratedRecHitCompressed {
public:
  typedef DetId key_type;

  enum Flags {
    kGood = -1,  // channel is good (mutually exclusive with other states)  setFlagBit(kGood) reset flags_ to zero
    kPoorReco,   // channel has been badly reconstructed (e.g. bad shape, bad chi2 etc.)
    kSaturated,  // saturated channel
    kOutOfTime   // channel out of time
  };

  HGCUncalibratedRecHitCompressed();
  HGCUncalibratedRecHitCompressed(
      const DetId& detId, float ampl, float ped, float jit, float chi2, uint32_t flags = 0, uint32_t aux = 0);
  explicit HGCUncalibratedRecHitCompressed(const HGCUncalibratedRecHit& hit);

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
  DetId id() const { return id_; }


private:
  friend class HGCUncalibratedRecHitCompressedsSorted;

  void setRawId(uint32_t id) { id_ = DetId(id); }

  float amplitude_;     //< Reconstructed amplitude
  float pedestal_;      //< Reconstructed pedestal
  float jitter_;        //< Reconstructed time jitter
  float chi2_;          //< Chi2 of the pulse
  float OOTamplitude_;  //< Out-Of-Time reconstructed amplitude
  float OOTchi2_;       //< Out-Of-Time Chi2
  uint32_t flags_;      //< flag to be propagated to RecHit
  uint32_t aux_;        //< aux word; first 8 bits contain time (jitter) error
  DetId id_;            //< Detector ID
};

#endif
