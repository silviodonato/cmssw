#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalToaPacking_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalToaPacking_h

#include <cstddef>
#include <cstdint>
#include <vector>

// Byte stream holding the 10-bit ToAs of the rows with the ToA-valid bit set, in row order.
// The n low bytes come first, followed by the 2-bit high parts packed four per byte (ToA j in bits 2*(j%4)).
// Splitting the bytes this way compresses better than one uint16_t per ToA.
namespace hgcaldigi::toa {
  inline std::size_t packedSize(std::size_t count) { return count + (count + 3) / 4; }

  inline std::vector<uint8_t> pack(std::vector<uint16_t> const& toas) {
    auto const count = toas.size();
    std::vector<uint8_t> packed(packedSize(count), 0);
    for (std::size_t j = 0; j < count; ++j) {
      packed[j] = toas[j] & 0xff;
      packed[count + j / 4] |= ((toas[j] >> 8) & 0x3) << (2 * (j % 4));
    }
    return packed;
  }

  // Returns ToA j of a stream holding count ToAs; the caller checks packed.size() == packedSize(count).
  inline uint16_t unpack(std::vector<uint8_t> const& packed, std::size_t count, std::size_t j) {
    return packed[j] | (((packed[count + j / 4] >> (2 * (j % 4))) & 0x3) << 8);
  }
}  // namespace hgcaldigi::toa

#endif
