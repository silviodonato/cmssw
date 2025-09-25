#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include <algorithm>
#include <cmath>
#include <assert.h>

SiStripApproximateCluster::SiStripApproximateCluster(const SiStripCluster& cluster,
                                                     unsigned int maxNSat,
                                                     float hitPredPos,
                                                     bool peakFilter,
                                                     bool v2) {

  
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
    // Represent value [0, avgChargeMax_=255] -->  [-0.5, avgChargeRangeMax_=63 + 0.5], convert to int
    avgCharge_ = round(float(cluster.charge()) / cluster.size()* float(avgChargeRangeMax_)/float(avgChargeMax_) );
    // In v2, we encode the filter_ and peakFilter_ info in avgCharge_ as the two highest bits
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_-2)) - 1) && "Setting avgCharge > 63");
    avgCharge_ = (avgCharge_ | (filter_ << kfilterMask));
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_-1)) - 1) && "Setting avgCharge > 127");
    avgCharge_ = (avgCharge_ | (peakFilter_ << kpeakFilterMask));
    assert(avgCharge_ <= ((1 <<  (nbits_avgCharge_)) - 1) && "Setting avgCharge > 255");

    // We encode the isSaturated_ info in barycenter_ as the highest bit
    int previous_cluster = 0;
    int module_length = 0;
    int previous_module_length = 0;
    barycenter_ = std::round(((cluster.barycenter()-previous_cluster)+(module_length-previous_module_length))* float(barycenterRangeMax_)/float(barycenterMax_));
    // std::cout<<"cluster.barycenter() "<<cluster.barycenter()<<std::endl;
    // std::cout<<"barycenterRangeMax_ "<<barycenterRangeMax_<<std::endl;
    // std::cout<<"barycenterMax_ "<<barycenterMax_<<std::endl;
    // std::cout<<"barycenter_ "<<barycenter_<<std::endl;
    assert(barycenter_ <= ((1 <<  (nbits_barycenter_-1)) - 1) && "Setting barycenter > 32767");
    // std::cout<<"barycenter_ "<<barycenter_<<std::endl;
    barycenter_ = (barycenter_ | (isSaturated_ << kSaturatedMask));
    // std::cout<<"barycenter_ "<<barycenter_<<std::endl;
    // std::cout<<"isSaturated_ "<<isSaturated_<<std::endl;
    // std::cout<<"kSaturatedMask "<<kSaturatedMask<<std::endl;
    // std::cout<<"nbits_barycenter_ "<<nbits_barycenter_<<std::endl;
    // std::cout<<" 1 <<  (nbits_barycenter_-1) "<< ((1 <<  nbits_barycenter_) -1)<<std::endl;
    assert(barycenter_ <= ((1 <<  nbits_barycenter_) -1) && "Setting barycenter > 65535");

    // We set the flags to false to reduce event size (they should be removed in 2026)
    filter_ = false;
    isSaturated_ = false;
    peakFilter_ = false;
  }
}
