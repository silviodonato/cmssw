#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalDigiInstances_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalDigiInstances_h

#include <array>
#include <string>
#include <string_view>

#include "FWCore/Utilities/interface/InputTag.h"

namespace hgcaldigi {
  inline constexpr std::array<std::string_view, 3> digiInstances = {"EE", "HEfront", "HEback"};
  inline constexpr std::array<std::string_view, 3> geometryNames = {
      "HGCalEESensitive", "HGCalHESiliconSensitive", "HGCalHEScintillatorSensitive"};
  // DetId bits below this shift identify a cell inside its group: cellU and cellV in a silicon wafer,
  // iphi in a scintillator ring. Used to group cells for the occupancy coding.
  inline constexpr std::array<unsigned, 3> cellGroupShifts = {10, 10, 9};

  inline edm::InputTag withInstance(edm::InputTag const& source, std::string_view instance) {
    return edm::InputTag(source.label(), std::string(instance), source.process());
  }
}  // namespace hgcaldigi

#endif
