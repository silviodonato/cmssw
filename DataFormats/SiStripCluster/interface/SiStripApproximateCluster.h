#ifndef DataFormats_SiStripCluster_SiStripApproximateCluster_h
#define DataFormats_SiStripCluster_SiStripApproximateCluster_h

#include "FWCore/Utilities/interface/typedefs.h"

class SiStripCluster;
class SiStripApproximateCluster {
public:
  SiStripApproximateCluster() {}

  explicit SiStripApproximateCluster(cms_uint16_t barycenter,
                                     cms_uint8_t width,
                                     cms_uint8_t avgCharge,
                                     bool filter,
                                     bool isSaturated,
                                     bool peakFilter = false,
                                     bool v2 = false)
      : barycenter_(barycenter),
        width_(width),
        avgCharge_(avgCharge),
        filter_(filter),
        isSaturated_(isSaturated),
        peakFilter_(peakFilter),
        v2_(v2) {}

  explicit SiStripApproximateCluster(const SiStripCluster& cluster,
                                     unsigned int maxNSat,
                                     float hitPredPos,
                                     bool peakFilter,
                                     bool v2 = false);

  //barycenter() gives barycenter position in tenths of strip (i.e. 10 means center of strip 1) (0-1536)
  //avgCharge() gives the average charge in ADC counts (0-255)
  //width() gives the cluster width (0-255)
  //v2() gives true if the cluster is in the new format (Fall 2025)
  cms_uint16_t barycenter() const {
  if (!v2_) return barycenter_;
  else {
    return barycenter_; 
  }
}
  cms_uint8_t width() const { return width_; }
  cms_uint8_t avgCharge() const { 
    if (!v2_) return avgCharge_;
    else {
      return avgCharge_;
    }
  } 
  bool filter() const { 
    if (!v2_) return filter_; 
    else {
      return filter_;
    }
  }
  bool isSaturated() const { 
    if (!v2_) return isSaturated_; 
    else {
      return isSaturated_;
    }
  }
  bool peakFilter() const { 
    if (!v2_) return peakFilter_; 
    else {
      return peakFilter_;
    }
  }
  bool v2() const { return v2_; }

private:
  cms_uint16_t barycenter_ = 0;
  cms_uint8_t width_ = 0;
  cms_uint8_t avgCharge_ = 0;
  bool filter_ = false;
  bool isSaturated_ = false;
  bool peakFilter_ = false;
  // v2 --> new version 
  bool v2_ = false;
  static constexpr double maxRange_ = 32767;
  static constexpr double maxBarycenter_ = 1536.;
  static constexpr double maxavgChargeRange_ = 63;
  static constexpr double maxavgCharge_ = 255.;
  static constexpr double trimMaxADC_ = 30.;
  static constexpr double trimMaxFracTotal_ = .15;
  static constexpr double trimMaxFracNeigh_ = .25;
  static constexpr double maxTrimmedSizeDiffNeg_ = .7;
  static constexpr double maxTrimmedSizeDiffPos_ = 1.;
  static constexpr int kfilterMask = 6;
  static constexpr int kpeakFilterMask = 7;
  static constexpr int kSaturatedMask = 15;
};
#endif  // DataFormats_SiStripCluster_SiStripApproximateCluster_h
