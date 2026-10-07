#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiInstances_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalLegacyDigiInstances_h

#include <array>
#include <string>
#include <string_view>

#include "FWCore/Utilities/interface/InputTag.h"

namespace hgcaldigi {
  inline constexpr std::array<std::string_view, 3> legacyDigiInstances = {"EE", "HEfront", "HEback"};

  inline edm::InputTag withInstance(edm::InputTag const& source, std::string_view instance) {
    return edm::InputTag(source.label(), std::string(instance), source.process());
  }

  inline std::string sampleExceptionsInstance(std::string_view instance) {
    return std::string(instance) + "Exceptions";
  }

  inline std::string detIdExceptionsInstance(std::string_view instance) {
    return std::string(instance) + "DetIdExceptions";
  }
}  // namespace hgcaldigi

#endif
