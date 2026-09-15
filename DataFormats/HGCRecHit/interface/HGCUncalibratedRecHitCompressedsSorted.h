#ifndef DATAFORMATS_HGCRECHIT_HGCUNCALIBRATEDRECHITCOMPRESSEDS_SORTED_H
#define DATAFORMATS_HGCRECHIT_HGCUNCALIBRATEDRECHITCOMPRESSEDS_SORTED_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "DataFormats/Common/interface/CMS_CLASS_VERSION.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"

// The deliberately plural name preserves the friendly name of the previous
// edm::SortedCollection<HGCUncalibratedRecHitCompressed> product.
class HGCUncalibratedRecHitCompressedsSorted {
public:
  using value_type = HGCUncalibratedRecHitCompressed;
  using key_type = DetId;
  using size_type = std::vector<value_type>::size_type;

  class const_iterator {
  public:
    const value_type& operator*() const;
    const value_type* operator->() const { return &operator*(); }

    const_iterator& operator++();
    const_iterator operator++(int) {
      const_iterator copy = *this;
      ++(*this);
      return copy;
    }

    bool operator==(const const_iterator& other) const { return current_ == other.current_; }
    bool operator!=(const const_iterator& other) const { return !(*this == other); }

  private:
    friend class HGCUncalibratedRecHitCompressedsSorted;
    const_iterator(std::vector<value_type>::const_iterator current, uint32_t previousId)
        : current_(current), previousId_(previousId) {}

    std::vector<value_type>::const_iterator current_;
    uint32_t previousId_ = 0;
    mutable value_type decodedHit_;
  };

  HGCUncalibratedRecHitCompressedsSorted() = default;

  void reserve(size_type size) { hits_.reserve(size); }
  bool empty() const { return hits_.empty(); }
  size_type size() const { return hits_.size(); }

  // Hits are supplied with absolute DetIds. post_insert(), called by
  // edm::Event::put(), sorts them and changes the stored IDs into deltas.
  void push_back(const value_type& hit) { hits_.push_back(hit); }

  const_iterator begin() const { return const_iterator(hits_.begin(), 0); }
  const_iterator cbegin() const { return begin(); }
  const_iterator end() const { return const_iterator(hits_.end(), 0); }
  const_iterator cend() const { return end(); }

  // This is called by edm::Event when the product is inserted. The input
  // uncalibrated-hit collection is already ordered, but sorting here makes
  // the delta representation robust to insertion order.
  void post_insert();

  CMS_CLASS_VERSION(1)

private:
  std::vector<value_type> hits_;
  bool deltaEncoded_ = false;
};

#endif
