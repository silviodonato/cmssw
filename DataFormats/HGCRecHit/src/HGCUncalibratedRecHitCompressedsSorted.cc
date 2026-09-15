#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressedsSorted.h"

const HGCUncalibratedRecHitCompressedsSorted::value_type&
HGCUncalibratedRecHitCompressedsSorted::const_iterator::operator*() const {
  decodedHit_ = *current_;
  decodedHit_.setRawId(previousId_ + current_->id().rawId());
  return decodedHit_;
}

HGCUncalibratedRecHitCompressedsSorted::const_iterator&
HGCUncalibratedRecHitCompressedsSorted::const_iterator::operator++() {
  previousId_ += current_->id().rawId();
  ++current_;
  return *this;
}

void HGCUncalibratedRecHitCompressedsSorted::post_insert() {
  if (deltaEncoded_)
    return;

  std::sort(hits_.begin(), hits_.end(), [](const value_type& left, const value_type& right) {
    return left.id() < right.id();
  });

  uint32_t previousId = 0;
  for (auto& hit : hits_) {
    const uint32_t id = hit.id().rawId();
    hit.setRawId(id - previousId);
    previousId = id;
  }
  deltaEncoded_ = true;
}
