#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/HGCalCommonData/interface/HGCalCell.h"
#include "Geometry/HGCalCommonData/interface/HGCalTypes.h"
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

class HGCalDigiGeometryExport : public edm::one::EDAnalyzer<> {
public:
  explicit HGCalDigiGeometryExport(edm::ParameterSet const& p)
      : digis_(consumes<HGCalDigiCollection>(p.getParameter<edm::InputTag>("src"))),
        geometry_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag("", p.getParameter<std::string>("geometryLabel")))),
        output_(p.getParameter<std::string>("output")) {}
  void analyze(edm::Event const& event, edm::EventSetup const& setup) override {
    auto const& geom = setup.getData(geometry_);
    auto const& digis = event.get(digis_);
    auto const& ddd = geom.topology().dddConstants();
    std::ofstream out(output_);
    if (!out) throw cms::Exception("FileWriteError") << output_;
    out << std::fixed << std::setprecision(5);
    out << "{\"event\":\"" << event.id().run() << ':' << event.luminosityBlock() << ':' << event.id().event()
        << "\",\"geometry\":\"D128\",\"schema\":2,\"cells\":[";
    bool first = true;
    for (auto const& digi : digis) {
      HGCSiliconDetId id(digi.id());
      if (!geom.topology().valid(id))
        throw cms::Exception("InvalidDetId") << "Digi " << id.rawId() << " is not valid in D128";
      auto pos = geom.getPosition(id);
      // Draw nominal lattice hexagons, not full hexagons about boundary-cell COGs.
      // HD200 (type 3) must use n=12, just like HD120; cellSizeHex(type=3)
      // in this release incorrectly selects the coarse size for getCorners().
      bool fine = id.type() == HGCSiliconDetId::HGCalHD120 || id.type() == HGCSiliconDetId::HGCalHD200;
      int n = fine ? HGCSiliconDetId::HGCalHighDensityN : HGCSiliconDetId::HGCalLowDensityN;
      // Draw the true nominal cell footprint: lattice hexagons about nominal
      // centres tile the wafer without gaps.
      constexpr double kDisplayScale = 1.0;
      double radius = kDisplayScale * ddd.waferSize(true) / (3 * n);
      auto nominal = geom.getPosition(id, false, false);
      // Do not take corner directions from getCorners(): on the -z endcap its
      // corners are not centred on the nominal position, so normalised
      // directions all point the same way and the hexagon collapses to a
      // sliver.  Silicon cells have corners at k*60 degrees in global x-y on
      // both endcaps (rotations by 60 degrees and x mirroring preserve this).
      if (!std::isfinite(pos.x()) || !std::isfinite(pos.y()))
        throw cms::Exception("InvalidGeometry") << id.rawId();
      if (!first) out << ',';
      first = false;
      out << '[' << id.rawId() << ',' << id.layer() << ',' << id.zside() << ','
          << id.waferU() << ',' << id.waferV() << ',' << id.cellU() << ',' << id.cellV() << ','
          << pos.x() << ',' << pos.y() << ',' << pos.z() << ",[";
      for (unsigned i = 0; i < 6; ++i) {
        if (i) out << ',';
        double phi = i * M_PI / 3;
        out << '[' << nominal.x() + radius * std::cos(phi) << ',' << nominal.y() + radius * std::sin(phi) << ']';
      }
      out << "],[";
      for (int i = 0; i < digi.size(); ++i) {
        if (i) out << ',';
        auto const& sample = digi.sample(i);
        out << '[' << sample.data() << ',' << (sample.mode() ? 1 : 0) << ',' << sample.toa() << ']';
      }
      out << "]]";
    }
    out << "]}";
    if (!out) throw cms::Exception("FileWriteError") << output_;
  }
private:
  edm::EDGetTokenT<HGCalDigiCollection> digis_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geometry_;
  std::string output_;
};
DEFINE_FWK_MODULE(HGCalDigiGeometryExport);
