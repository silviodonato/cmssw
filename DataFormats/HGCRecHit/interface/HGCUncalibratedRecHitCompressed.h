#ifndef DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED
#define DATAFORMATS_HGCUNCALIBRATEDRECHITCOMPRESSED

#include <cstdint>
#include <limits>
#include <vector>
#include "DataFormats/DetId/interface/DetId.h"

class HGCUncalibratedRecHit;
class HGCUncalibratedRecHitCompressed {
public:
  typedef DetId key_type;
  using index_type = uint8_t;
  using amplitude_type = uint16_t;
  using jitter_type = uint16_t;
  // HGCSample::data() is not clamped by the weights reconstruction. The
  // digitizer can therefore provide the ADC saturation code 1024 in addition
  // to the nominal 10-bit range 0..1023.
  static constexpr amplitude_type kADCCodeCount = 1025;
  static constexpr amplitude_type kTDCCodeCount = 4096;
  static constexpr amplitude_type kMaximumAmplitudeCode = kADCCodeCount + kTDCCodeCount - 1;
  // This value is outside the 13-bit ADC/TDC code space and is used only by
  // geometry-index padding entries.
  static constexpr amplitude_type kPaddingAmplitudeCode = std::numeric_limits<amplitude_type>::max();
  static constexpr jitter_type kFakeJitterCode = 0;    // Fake jitter code for hits with no valid ToA (currently -99 in the uncompressed rechit)
  static constexpr jitter_type kJitterOffset = kFakeJitterCode+1;       //< Encoded ToA integer offset

  enum Flags {
    kGood = -1,  // channel is good (mutually exclusive with other states)  setFlagBit(kGood) reset flags_ to zero
    kPoorReco,   // channel has been badly reconstructed (e.g. bad shape, bad chi2 etc.)
    kSaturated,  // saturated channel
    kOutOfTime   // channel out of time
  };

  HGCUncalibratedRecHitCompressed();
  HGCUncalibratedRecHitCompressed(const HGCUncalibratedRecHit& hit,
                                  index_type geometryIndex,
                                  double tofDelay,
                                  double toaLSB_ns);

  // A padding hit advances the geometry-index delta stream and is discarded
  // by the decompressor. Its payload uses the common/default values and an
  // amplitude code outside the physical ADC/TDC range.
  static HGCUncalibratedRecHitCompressed makeIndexPadding(index_type geometryIndex) {
    HGCUncalibratedRecHitCompressed padding;
    padding.amplitude_ = kPaddingAmplitudeCode;
    padding.pedestal_ = -1.f;
    padding.jitter_ = kFakeJitterCode;
    padding.chi2_ = -1.f;
    padding.OOTchi2_ = 10000.f;
    padding.id_ = geometryIndex;
    return padding;
  }

  virtual ~HGCUncalibratedRecHitCompressed();
  amplitude_type amplitudeCode() const { return amplitude_; }
  bool isIndexPadding() const { return amplitude_ == kPaddingAmplitudeCode; }
  static amplitude_type encodeAmplitude(float amplitude, const DetId& id);
  static float decodeAmplitude(amplitude_type code, const DetId& id);
  float pedestal() const { return pedestal_; }
  // The persisted value is the encoded ToA integer minus kJitterOffset.
  jitter_type jitterIndex() const { return jitter_; }
  jitter_type jitterInteger() const { return jitter_; }
  float jitter(double tofDelay, double toaLSB_ns) const;
  float chi2() const { return chi2_; }
  float outOfTimeEnergy() const { return OOTamplitude_; }
  float outOfTimeChi2() const { return OOTchi2_; }

  uint32_t flags() const { return flags_; }
  uint32_t aux() const { return aux_; }
  float jitterError() const;
  uint8_t jitterErrorBits() const;
  DetId id() const { return DetId(id_); }

private:
  amplitude_type amplitude_;  //< ADC/TDC code: [0,1024] ADC, [1025,5120] TDC
  jitter_type jitter_;  //< Encoded ToA integer minus kJitterOffset (10-bit range)
  float pedestal_;      //< Reconstructed pedestal
  float chi2_;          //< Chi2 of the pulse
  float OOTamplitude_;  //< Out-Of-Time reconstructed amplitude
  float OOTchi2_;       //< Out-Of-Time Chi2
  uint32_t flags_;      //< flag to be propagated to RecHit
  uint32_t aux_;        //< aux word; first 8 bits contain time (jitter) error
  index_type id_;       //< Index in the valid-DetId list, or its delta
};

#endif
