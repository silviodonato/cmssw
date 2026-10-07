#include <cstdint>
#include <iostream>
#include <random>

#include "DataFormats/HGCalDigi/interface/HGCalDigiCompressedHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"

// Fills the same in-time-sample content in both layouts and checks that every accessor agrees.
int main() {
  constexpr int size = 100000;
  std::mt19937 rng(12345);
  hgcaldigi::HGCalDigiHost reference(size);
  hgcaldigi::HGCalDigiCompressedHost compressed(size);
  auto referenceView = reference.view();
  auto compressedView = compressed.view();

  for (int i = 0; i < size; ++i) {
    bool const mode = rng() & 1;
    bool const threshold = rng() & 1;
    bool const toaValid = rng() & 1;
    uint16_t const data = rng() & 0xfff;
    uint16_t const toa = toaValid ? rng() & 0x3ff : 0;  // includes ToA == 0, which must stay valid
    uint8_t const tctp = (mode ? 1 : 0) | (threshold ? 2 : 0) | (toaValid ? 4 : 0);

    auto row = referenceView[i];
    row.tctp() = tctp;
    row.adcm1() = 0;
    row.adc() = mode ? 0 : data;
    row.tot() = mode ? data : 0;
    row.toa() = toa;
    row.cm() = 0;
    row.flags() = hgcal::DIGI_FLAG::Invalid;

    auto packedRow = compressedView[i];
    // Setting tctp before and after the data must not alter either of them.
    packedRow.setTctp(tctp);
    if (mode)
      packedRow.setTot(data);
    else
      packedRow.setAdc(data);
    packedRow.setTctp(tctp);
    if (toaValid)
      packedRow.setToa(toa);
    else
      packedRow.clearToa();
  }

  auto const constView = compressed.const_view();
  for (int i = 0; i < size; ++i) {
    auto const a = reference.const_view()[i];
    auto const b = constView[i];
    if (a.tctp() != b.tctp() || a.adcm1() != b.adcm1() || a.adc() != b.adc() || a.tot() != b.tot() ||
        a.toa() != b.toa() || a.cm() != b.cm() || a.flags() != b.flags()) {
      std::cerr << "Mismatch at row " << i << std::endl;
      return 1;
    }
  }
  static_assert(sizeof(uint16_t) * 2 < sizeof(uint8_t) + 6 * sizeof(uint16_t));
  std::cout << "HGCalDigiCompressedSoA matches HGCalDigiSoA for " << size << " rows" << std::endl;
  return 0;
}
