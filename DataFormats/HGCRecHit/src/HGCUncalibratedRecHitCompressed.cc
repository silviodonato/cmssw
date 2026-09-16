#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHit.h"
#include <cmath>
#include <stdexcept>

namespace {
constexpr float kNoJitter = -99.f;
}

HGCUncalibratedRecHitCompressed::HGCUncalibratedRecHitCompressed()
    : amplitude_(0.),
      pedestal_(0.),
      jitter_(0.),
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
    : amplitude_(hit.amplitude()),
      pedestal_(hit.pedestal()),
      jitter_(0),
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

float HGCUncalibratedRecHitCompressed::jitter(double tofDelay, double toaLSB_ns) const {
  if (jitterInteger() == kFakeJitterCode)
    return kNoJitter;
  return static_cast<float>(-tofDelay + static_cast<double>(jitterInteger() - kJitterOffset) * toaLSB_ns);
}

HGCUncalibratedRecHitCompressed::~HGCUncalibratedRecHitCompressed() {}
