#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCUncalibratedRecHitCompressed.h"

#include <array>
#include <cmath>
#include <tuple>
#include <utility>

#include <catch2/catch_all.hpp>

namespace {
constexpr double kADCLeastSignificantBit_fC = 100.0 / 1024.0;
constexpr double kTDCLeastSignificantBit_fC = 10000.0 / 4096.0;
constexpr double kTDCOnset_fC = 60.0;
const double kTDCBase_fC =
    (std::floor(kTDCOnset_fC / kADCLeastSignificantBit_fC) + 1.0) * kADCLeastSignificantBit_fC;
constexpr std::array<double, 4> kFCPerMIP = {2.06, 3.43, 5.15, 3.43};

float reconstructedAmplitude(uint16_t data, bool tdc, int type) {
  const double charge = tdc ? kTDCBase_fC + (static_cast<double>(data) + 0.5) * kTDCLeastSignificantBit_fC
                            : static_cast<double>(data) * kADCLeastSignificantBit_fC;
  return static_cast<float>(charge / kFCPerMIP[type]);
}

HGCSiliconDetId eeId(int type) {
  return HGCSiliconDetId(DetId::HGCalEE, 1, type, 1, 0, 0, 0, 0);
}
}  // namespace

TEST_CASE("HGCEE amplitude ADC/TDC compression is lossless", "[HGCUncalibratedRecHitCompressed]") {
  for (int type = 0; type < static_cast<int>(kFCPerMIP.size()); ++type) {
    const auto id = eeId(type);

    for (uint16_t data = 0; data < HGCUncalibratedRecHitCompressed::kADCCodeCount; ++data) {
      const float amplitude = reconstructedAmplitude(data, false, type);
      const auto code = HGCUncalibratedRecHitCompressed::encodeAmplitude(amplitude, id);
      REQUIRE(code == data);
      REQUIRE(code <= HGCUncalibratedRecHitCompressed::kMaximumAmplitudeCode);
      REQUIRE(HGCUncalibratedRecHitCompressed::decodeAmplitude(code, id) == amplitude);
    }

    for (uint16_t data = 0; data < HGCUncalibratedRecHitCompressed::kTDCCodeCount; ++data) {
      const float amplitude = reconstructedAmplitude(data, true, type);
      const auto code = HGCUncalibratedRecHitCompressed::encodeAmplitude(amplitude, id);
      REQUIRE(code == HGCUncalibratedRecHitCompressed::kADCCodeCount + data);
      REQUIRE(code <= HGCUncalibratedRecHitCompressed::kMaximumAmplitudeCode);
      REQUIRE(HGCUncalibratedRecHitCompressed::decodeAmplitude(code, id) == amplitude);
    }
  }
}

TEST_CASE("HGCEE amplitude compression preserves the ADC/TDC boundary", "[HGCUncalibratedRecHitCompressed]") {
  const auto id = eeId(0);
  const float lastADC = reconstructedAmplitude(HGCUncalibratedRecHitCompressed::kADCCodeCount - 1, false, 0);
  const float firstTDC = reconstructedAmplitude(0, true, 0);

  REQUIRE(HGCUncalibratedRecHitCompressed::encodeAmplitude(lastADC, id) ==
          HGCUncalibratedRecHitCompressed::kADCCodeCount - 1);
  REQUIRE(HGCUncalibratedRecHitCompressed::encodeAmplitude(firstTDC, id) ==
          HGCUncalibratedRecHitCompressed::kADCCodeCount);
  REQUIRE(HGCUncalibratedRecHitCompressed::decodeAmplitude(HGCUncalibratedRecHitCompressed::kADCCodeCount - 1, id) ==
          lastADC);
  REQUIRE(HGCUncalibratedRecHitCompressed::decodeAmplitude(HGCUncalibratedRecHitCompressed::kADCCodeCount, id) ==
          firstTDC);
}

TEST_CASE("HGCHEF and HGCHEB amplitude compression use their HLT settings", "[HGCUncalibratedRecHitCompressed]") {
  const HGCSiliconDetId hefId(DetId::HGCalHSi, 1, 1, 1, 0, 0, 0, 0);
  const HGCScintillatorDetId hebId(1, 1, 1, 1);
  constexpr std::array<std::pair<bool, uint16_t>, 4> codeCases = {
      {{false, 0}, {false, 1024}, {true, 0}, {true, 4095}}};

  for (const auto& [id, adcLSB, tdcLSB, tdcOnset, fCPerMIP] :
       {std::tuple<DetId, double, double, double, double>{hefId, 100.0 / 1024.0, 10000.0 / 4096.0, 60.0, 3.43},
        std::tuple<DetId, double, double, double, double>{hebId, 68.75 / 1024.0, 1000.0 / 4096.0, 55.0, 1.0}}) {
    const double tdcBase = (std::floor(tdcOnset / adcLSB) + 1.0) * adcLSB;
    for (const auto& [tdc, data] : codeCases) {
      const double charge = tdc ? tdcBase + (static_cast<double>(data) + 0.5) * tdcLSB
                                : static_cast<double>(data) * adcLSB;
      const float amplitude = static_cast<float>(charge / fCPerMIP);
      const auto code = HGCUncalibratedRecHitCompressed::encodeAmplitude(amplitude, id);
      REQUIRE(code == (tdc ? HGCUncalibratedRecHitCompressed::kADCCodeCount + data : data));
      REQUIRE(HGCUncalibratedRecHitCompressed::decodeAmplitude(code, id) == amplitude);
    }
  }
}
