#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <stdio.h>

SiStripApproximateCluster::SiStripApproximateCluster(const SiStripCluster& cluster,
                                                     unsigned int maxNSat,
                                                     float hitPredPos,
                                                     bool peakFilter,
                                                     bool v2,
                                                     float previous_cluster,
                                                     unsigned int offset_module_change)
                                                     {

  
  barycenter_ = std::round(cluster.barycenter() * 10);
  width_ = cluster.size();
  avgCharge_ = cluster.charge() / cluster.size();
  filter_ = false;
  isSaturated_ = false;
  peakFilter_ = peakFilter;
  v2_ = v2;

  //mimicing the algorithm used in StripSubClusterShapeTrajectoryFilter...
  //Looks for 3 adjacent saturated strips (ADC>=254)
  const auto& ampls = cluster.amplitudes();
  unsigned int thisSat = (ampls[0] >= 254), maxSat = thisSat;
  for (unsigned int i = 1, n = ampls.size(); i < n; ++i) {
    if (ampls[i] >= 254) {
      thisSat++;
    } else if (thisSat > 0) {
      maxSat = std::max<int>(maxSat, thisSat);
      thisSat = 0;
    }
  }
  if (thisSat > 0) {
    maxSat = std::max<int>(maxSat, thisSat);
  }
  if (maxSat >= maxNSat) {
    filter_ = true;
    isSaturated_ = true;
  }

  unsigned int hitStripsTrim = ampls.size();
  int sum = std::accumulate(ampls.begin(), ampls.end(), 0);
  uint8_t trimCut = std::min<uint8_t>(trimMaxADC_, std::floor(trimMaxFracTotal_ * sum));
  auto begin = ampls.begin();
  auto last = ampls.end() - 1;
  while (hitStripsTrim > 1 && (*begin < std::max<uint8_t>(trimCut, trimMaxFracNeigh_ * (*(begin + 1))))) {
    hitStripsTrim--;
    ++begin;
  }
  while (hitStripsTrim > 1 && (*last < std::max<uint8_t>(trimCut, trimMaxFracNeigh_ * (*(last - 1))))) {
    hitStripsTrim--;
    --last;
  }
  if (hitStripsTrim < std::floor(std::abs(hitPredPos) - maxTrimmedSizeDiffNeg_)) {
    filter_ = false;
  } else if (hitStripsTrim <= std::ceil(std::abs(hitPredPos) + maxTrimmedSizeDiffPos_)) {
    filter_ = true;
  } else {
    filter_ = peakFilter_;
  }

  if (v2_) {
    // Map value [0, avgChargeMax_=255] -->  [0, ..., 63], convert to int
    //Floor are used to avoid rounding issues of int numbers
    avgCharge_ = floor(float(cluster.charge()) / cluster.size() / floor(avgChargeMax_/avgChargeRangeMax_) );
    // In v2, we encode the filter_ and peakFilter_ info in avgCharge_ as the two highest bits
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_-2)) - 1) && "Setting avgCharge > 63");
    avgCharge_ = (avgCharge_ | (filter_ << kfilterMask));
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_-1)) - 1) && "Setting avgCharge > 127");
    avgCharge_ = (avgCharge_ | (peakFilter_ << kpeakFilterMask));
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_)) - 1) && "Setting avgCharge > 255");

    // We encode the isSaturated_ info in barycenter_ as the highest bit

    barycenter_ = round(float(cluster.barycenter()-previous_cluster + (offset_module_change)) * (2*floor(0.5*barycenterRangeMax_/barycenterMax_)));
    assert(barycenter_ <= ((1 <<  (nbits_barycenter_-1)) - 1) && "Setting barycenter > 32767");
    barycenter_ = (barycenter_ | (isSaturated_ << kSaturatedMask));
    assert(barycenter_ <= ((1 <<  nbits_barycenter_) -1) && "Setting barycenter > 65535");

    // We set the flags to false to reduce event size (they should be removed in 2026)
    filter_ = false;
    isSaturated_ = false;
    peakFilter_ = false;
  }
}
