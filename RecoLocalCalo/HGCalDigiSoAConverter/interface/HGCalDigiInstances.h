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

  inline edm::InputTag withInstance(edm::InputTag const& source, std::string_view instance) {
    return edm::InputTag(source.label(), std::string(instance), source.process());
  }

  inline std::string sidecarInstance(std::string_view instance, std::string_view suffix) {
    return std::string(instance) + std::string(suffix);
  }
}  // namespace hgcaldigi

#endif
