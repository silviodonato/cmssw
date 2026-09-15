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

void HGCUncalibratedRecHitCompressedsSorted::push_back(const value_type& hit, uint32_t geometryIndex) {
  value_type indexedHit = hit;
  indexedHit.setRawId(geometryIndex);
  hits_.push_back(indexedHit);
}

void HGCUncalibratedRecHitCompressedsSorted::post_insert() {
  if (deltaEncoded_)
    return;

  std::sort(hits_.begin(), hits_.end(), [](const value_type& left, const value_type& right) {
    return left.id() < right.id();
  });

  uint32_t previousIndex = 0;
  for (auto& hit : hits_) {
    const uint32_t index = hit.id().rawId();
    hit.setRawId(index - previousIndex);
    previousIndex = index;
  }
  deltaEncoded_ = true;
}
