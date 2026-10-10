#ifndef DataFormats_HGCalDigi_interface_HGCalDigiCompressedSoA_h
#define DataFormats_HGCalDigi_interface_HGCalDigiCompressedSoA_h

#include <cstdint>

#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"

namespace hgcaldigi {

  // Bit layout of the `packed` column: [14:12] tctp (mode, threshold, ToA valid), [11:0] adc or tot.
  // tctp bit 0 (mode) selects the meaning of the 12-bit data: ToT if set, ADC otherwise.
  // The ToA is not stored in the SoA: only rows with the ToA-valid bit set have one, so the ToAs are kept
  // densely, in row order, in a separate product (see RecoLocalCalo/HGCalDigiSoAConverter).
  namespace compressed {
    constexpr uint16_t dataBits = 12;
    constexpr uint16_t dataMask = 0x0fff;
    constexpr uint16_t storedTctpMask = 0x7;
    constexpr uint16_t tctpShift = dataBits;
    constexpr uint8_t modeBit = 0x1;
    constexpr uint8_t toaValidBit = 0x4;
  }  // namespace compressed

  // Compact version of HGCalDigiSoALayout for a single in-time sample, as needed by the rec-hit algorithm.
  // adc and tot share the same 12 bits, because only one of them is meaningful in each row (selected by tctp).
  // adcm1, cm and flags are constant in this representation, so they are not stored.
  // The element methods mirror the HGCalDigiSoALayout columns: tctp(), adcm1(), adc(), tot(), cm(), flags().
  GENERATE_SOA_LAYOUT(
      HGCalDigiCompressedSoALayout,
      SOA_COLUMN(uint16_t, packed),

      SOA_ELEMENT_METHODS(
          // Set the mode, threshold and ToA-valid bits of tctp, leaving the 12-bit data unchanged.
          SOA_HOST_DEVICE void setTctp(uint8_t tctp) {
            packed() = (packed() & compressed::dataMask) |
                       static_cast<uint16_t>((tctp & compressed::storedTctpMask) << compressed::tctpShift);
          }

          // Set the 12-bit data and mark the row as ADC mode (tctp bit 0 cleared).
          SOA_HOST_DEVICE void setAdc(uint16_t adc) {
            uint16_t const tctpBits = packed() & ~compressed::dataMask & ~(compressed::modeBit << compressed::tctpShift);
            packed() = tctpBits | (adc & compressed::dataMask);
          }

          // Set the 12-bit data and mark the row as ToT mode (tctp bit 0 set).
          SOA_HOST_DEVICE void setTot(uint16_t tot) {
            uint16_t const tctpBits = (packed() & ~compressed::dataMask) | (compressed::modeBit << compressed::tctpShift);
            packed() = tctpBits | (tot & compressed::dataMask);
          }),

      SOA_CONST_ELEMENT_METHODS(
          SOA_HOST_DEVICE uint8_t tctp() const {
            return (packed() >> compressed::tctpShift) & compressed::storedTctpMask;
          }

          SOA_HOST_DEVICE bool toaValid() const { return tctp() & compressed::toaValidBit; }

          SOA_HOST_DEVICE uint16_t data() const { return packed() & compressed::dataMask; }

          SOA_HOST_DEVICE bool isTot() const { return tctp() & compressed::modeBit; }

          SOA_HOST_DEVICE uint16_t adc() const { return isTot() ? 0 : data(); }

          SOA_HOST_DEVICE uint16_t tot() const { return isTot() ? data() : 0; }

          SOA_HOST_DEVICE uint16_t adcm1() const { return 0; }

          SOA_HOST_DEVICE uint16_t cm() const { return 0; }

          SOA_HOST_DEVICE uint16_t flags() const { return hgcal::DIGI_FLAG::Invalid; }))

  using HGCalDigiCompressedSoA = HGCalDigiCompressedSoALayout<>;

}  // namespace hgcaldigi

#endif  // DataFormats_HGCalDigi_interface_HGCalDigiCompressedSoA_h
