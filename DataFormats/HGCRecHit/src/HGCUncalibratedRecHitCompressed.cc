#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHit.h"

#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"

#include <array>
#include <cmath>
#include <stdexcept>
#include <string>

namespace {
constexpr float kNoJitter = -99.f;

struct AmplitudeConfiguration {
  double adcLeastSignificantBit_fC;
  double tdcLeastSignificantBit_fC;
  double tdcOnset_fC;
};

// These are the Phase-2 V19 HLT settings from HGCalUncalibRecHit_cfi.py.
// Keep all amplitude arithmetic here so encoding and decoding use exactly the
// same operations as the reconstruction.
constexpr AmplitudeConfiguration kSiliconConfiguration = {100.0 / 1024.0, 10000.0 / 4096.0, 60.0};
constexpr AmplitudeConfiguration kScintillatorConfiguration = {68.75 / 1024.0, 1000.0 / 4096.0, 55.0};
constexpr std::array<double, 4> kFCPerMIP = {2.06, 3.43, 5.15, 3.43};

const AmplitudeConfiguration& amplitudeConfiguration(const DetId& id) {
  if (id.det() == DetId::HGCalEE || id.det() == DetId::HGCalHSi) {
    return kSiliconConfiguration;
  }
  if (id.det() == DetId::HGCalHSc) {
    return kScintillatorConfiguration;
  }
  throw std::invalid_argument("HGCal amplitude compression received an unsupported DetId");
}

double fCPerMIP(const DetId& id) {
  if (id.det() == DetId::HGCalHSc) {
    return 1.0;
  }
  const HGCSiliconDetId siliconId(id);
  if (!siliconId.isEE() && !siliconId.isHE()) {
    throw std::invalid_argument("HGCal amplitude compression received an unsupported silicon DetId");
  }
  const auto type = siliconId.type();
  if (type < 0 || type >= static_cast<int>(kFCPerMIP.size())) {
    throw std::invalid_argument("HGCal amplitude compression received an HGCEE DetId with an invalid wafer type");
  }
  return kFCPerMIP[type];
}

float reconstructAmplitude(HGCUncalibratedRecHitCompressed::amplitude_type code, const DetId& id) {
  if (code > HGCUncalibratedRecHitCompressed::kMaximumAmplitudeCode) {
    throw std::out_of_range("HGCal compressed amplitude code is outside the ADC/TDC range");
  }

  const auto& configuration = amplitudeConfiguration(id);
  const double tdcBase_fC =
      (std::floor(configuration.tdcOnset_fC / configuration.adcLeastSignificantBit_fC) + 1.0) *
      configuration.adcLeastSignificantBit_fC;
  const double charge =
      code < HGCUncalibratedRecHitCompressed::kADCCodeCount
          ? static_cast<double>(code) * configuration.adcLeastSignificantBit_fC
          : tdcBase_fC + (static_cast<double>(code - HGCUncalibratedRecHitCompressed::kADCCodeCount) + 0.5) *
                               configuration.tdcLeastSignificantBit_fC;
  return static_cast<float>(charge / fCPerMIP(id));
}
}

HGCUncalibratedRecHitCompressed::HGCUncalibratedRecHitCompressed()
    : amplitude_(0),
      jitter_(0),
      pedestal_(0.),
      chi2_(10000.),
      OOTamplitude_(0.),
      OOTchi2_(10000.),
      flags_(0),
      aux_(0),
      id_(0) {}

HGCUncalibratedRecHitCompressed::HGCUncalibratedRecHitCompressed(const HGCUncalibratedRecHit& hit,
                                                                 index_type geometryIndex,
                                                                 double tofDelay,
                                                                 double toaLSB_ns)
    : amplitude_(encodeAmplitude(hit.amplitude(), hit.id())),
      jitter_(0),
      pedestal_(hit.pedestal()),
      chi2_(hit.chi2()),
      OOTamplitude_(hit.outOfTimeEnergy()),
      OOTchi2_(hit.outOfTimeChi2()),
      flags_(hit.flags()),
      aux_(hit.aux()),
      id_(geometryIndex) {
  if (hit.jitter() == kNoJitter) {
    jitter_ = kFakeJitterCode;
    return;
  }
  if (!std::isfinite(hit.jitter()) || !std::isfinite(tofDelay) || !std::isfinite(toaLSB_ns) || toaLSB_ns <= 0.) {
    throw std::invalid_argument("HGCal jitter encoding requires a finite positive ToA LSB");
  }

  jitter_ = std::lround((static_cast<double>(hit.jitter()) + tofDelay) / toaLSB_ns) + kJitterOffset;
  if (jitter_ < static_cast<long>(std::numeric_limits<jitter_type>::min()) || jitter_ > static_cast<long>(std::numeric_limits<jitter_type>::max())) { //jitter_ < 0 always false since jitter_ is unsigned
    throw std::out_of_range("HGCal jitter does not fit in the compressed 10-bit representation"
                            "(original: " +
                            std::to_string(hit.jitter()) + ", tofDelay: " + std::to_string(tofDelay) +
                            ", toaLSB_ns: " + std::to_string(toaLSB_ns) + ", encoded: " + std::to_string(jitter_) + ")");
  }
}

HGCUncalibratedRecHitCompressed::amplitude_type HGCUncalibratedRecHitCompressed::encodeAmplitude(float amplitude,
                                                                                                   const DetId& id) {
  if (!std::isfinite(amplitude)) {
    throw std::invalid_argument("HGCal amplitude encoding requires a finite amplitude");
  }

  const double charge = static_cast<double>(amplitude) * fCPerMIP(id);
  const auto& configuration = amplitudeConfiguration(id);
  const double tdcBase_fC =
      (std::floor(configuration.tdcOnset_fC / configuration.adcLeastSignificantBit_fC) + 1.0) *
      configuration.adcLeastSignificantBit_fC;
  const double adcCandidate = charge / configuration.adcLeastSignificantBit_fC;
  if (adcCandidate >= -1. && adcCandidate <= kADCCodeCount) {
    const auto adc = std::lround(adcCandidate);
    if (adc >= 0 && adc < kADCCodeCount) {
      const auto code = static_cast<amplitude_type>(adc);
      if (reconstructAmplitude(code, id) == amplitude) {
        return code;
      }
    }
  }

  const double tdcCandidate = (charge - tdcBase_fC) / configuration.tdcLeastSignificantBit_fC - 0.5;
  if (tdcCandidate >= -1. && tdcCandidate <= kTDCCodeCount) {
    const auto tdc = std::lround(tdcCandidate);
    if (tdc >= 0 && tdc < kTDCCodeCount) {
      const auto code = static_cast<amplitude_type>(kADCCodeCount + tdc);
      if (reconstructAmplitude(code, id) == amplitude) {
        return code;
      }
    }
  }

  throw std::invalid_argument("HGCal amplitude cannot be represented exactly by the ADC/TDC code "
                              "(amplitude: " +
                              std::to_string(amplitude) + ", DetId: 0x" + std::to_string(id.rawId()) + ")");
}

float HGCUncalibratedRecHitCompressed::decodeAmplitude(amplitude_type code, const DetId& id) {
  return reconstructAmplitude(code, id);
}

float HGCUncalibratedRecHitCompressed::jitter(double tofDelay, double toaLSB_ns) const {
  if (jitterInteger() == kFakeJitterCode)
    return kNoJitter;
  return static_cast<float>(-tofDelay + static_cast<double>(jitterInteger() - kJitterOffset) * toaLSB_ns);
}

HGCUncalibratedRecHitCompressed::~HGCUncalibratedRecHitCompressed() {}
