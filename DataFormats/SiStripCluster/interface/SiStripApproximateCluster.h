#ifndef DataFormats_SiStripCluster_SiStripApproximateCluster_h
#define DataFormats_SiStripCluster_SiStripApproximateCluster_h

#include "FWCore/Utilities/interface/typedefs.h"
#include <climits>
#include <iostream>

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
  // else {
    // return barycenter_; 
  // }
  else {
     // Drop the first bit (encoding the saturation info)
     auto barycenter_decoded = (barycenter_ & 0b0111'1111'1111'1111); 
    //  std::cout<<" input barycenter_ "<<barycenter_<<std::endl;
    //  std::cout<<" decoded barycenter_decoded "<<barycenter_decoded<<std::endl;
    //  std::cout<<" output barycenter() "<<barycenter_decoded * 10. * float(barycenterMax_)/float(barycenterRangeMax_) <<std::endl;
     return barycenter_decoded * 10. * float(barycenterMax_)/float(barycenterRangeMax_) ;
     // the factor 10 is used for compatibility with v1, where barycenter() returned an integer in tenths of strips. It should be a float in the future instead.
  }
}
  cms_uint8_t width() const { return width_; }
  cms_uint8_t avgCharge() const {  // should be a float in the future instead of an int
    if (!v2_) return avgCharge_;
    else {
     // Drop the first two bits (encoding the filter and saturation info)
     cms_uint8_t avgCharge_decoded = (avgCharge_ & 0b0011'1111); 
     // Rescale avgCharge from  [0-63] (equivalent to [-0.5, 63.5]) to 0-255
     float avgCharge_rescaled = avgCharge_decoded * float(avgChargeMax_)/float(avgChargeRangeMax_);
     //assert(avgCharge_ <= avgChargeMax_ && "Returning avgCharge > maxavgCharge");
     return avgCharge_rescaled; }
  } 

  // If v2 is true, the filter_ and kpeakFilter_ info is encoded in avgCharge_
  bool filter() const { 
    if (!v2_) return filter_; 
    else  return (avgCharge_& (1<<kfilterMask));
  }
  bool peakFilter() const { 
    if (!v2_) return peakFilter_; 
    else return (avgCharge_ & (1<<kpeakFilterMask));
  }

  // If v2 is true, the peakFilter_ info is encoded in barycenter_
  bool isSaturated() const { 
    if (!v2_) return isSaturated_; 
    else return (barycenter_& (1<<kSaturatedMask));
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
  static constexpr double trimMaxADC_ = 30.;
  static constexpr double trimMaxFracTotal_ = .15;
  static constexpr double trimMaxFracNeigh_ = .25;
  static constexpr double maxTrimmedSizeDiffNeg_ = .7;
  static constexpr double maxTrimmedSizeDiffPos_ = 1.;

  ////// Encoding constants for v2 ///////////
  // maximum value of barycenter_ is 768 strips (128 strips/APV * 6 APVs)
  static constexpr double barycenterMax_ = 768.;
  // get the number of bits in barycenter_ (16 bits for cms_uint16_t)
  static constexpr int nbits_barycenter_ = sizeof(barycenter_) * CHAR_BIT;
  // position of the bit used to encode isSaturated_ in barycenter_
  static constexpr int kSaturatedMask = nbits_barycenter_-1;
  // get the largest number storable in barycenter_ with the remaining bits (2^15 -1 = 32767)
  static constexpr int barycenterRangeMax_ = (1 <<  (nbits_barycenter_-1)) - 1;

  // maximum value of avgCharge_ is 255 ADC counts
  static constexpr double avgChargeMax_ = 255.;
  // get the number of bits in avgCharge_ (8 bits for cms_uint8_t)
  static constexpr int nbits_avgCharge_ = sizeof(avgCharge_) * CHAR_BIT;
  // positions of the bit used to encode filter_ and peakFilter_ in avgCharge_
  static constexpr int kpeakFilterMask = nbits_avgCharge_-1;
  static constexpr int kfilterMask = nbits_avgCharge_-2;
  // get the largest number storable in avgCharge_ with the remaining bits (2^6 -1 = 63)
  static constexpr int avgChargeRangeMax_ = (1 <<  (nbits_avgCharge_-2)) - 1;
  ////////////////////////////////////////

};
#endif  // DataFormats_SiStripCluster_SiStripApproximateCluster_h
