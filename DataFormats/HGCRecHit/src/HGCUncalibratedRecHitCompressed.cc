#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHit.h"
#include <cmath>

HGCUncalibratedRecHitCompressed::HGCUncalibratedRecHitCompressed()
    : amplitude_(0.),
      pedestal_(0.),
      jitter_(0.),
      chi2_(10000.),
      OOTamplitude_(0.),
      OOTchi2_(10000.),
      flags_(0),
      aux_(0) {}

HGCUncalibratedRecHitCompressed::HGCUncalibratedRecHitCompressed(const HGCUncalibratedRecHit& hit)
    : amplitude_(hit.amplitude()),
      pedestal_(hit.pedestal()),
      jitter_(hit.jitter()),
      chi2_(hit.chi2()),
      OOTamplitude_(hit.outOfTimeEnergy()),
      OOTchi2_(hit.outOfTimeChi2()),
      flags_(hit.flags()),
      aux_(hit.aux()),
      id_(hit.id()) {}

HGCUncalibratedRecHitCompressed::~HGCUncalibratedRecHitCompressed() {}