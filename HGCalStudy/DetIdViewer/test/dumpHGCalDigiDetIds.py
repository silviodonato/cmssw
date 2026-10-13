#!/usr/bin/env python3
"""Print silicon HGCal digi DetIds for a layer using the CMSSW FWLite runtime."""

import argparse
import sys


def layer_argument(value):
    if value.lower() == "all":
        return "all"
    try:
        layer = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError("layer must be 1–31 or all")
    if not 1 <= layer <= 31:
        raise argparse.ArgumentTypeError("layer must be 1–31 or all")
    return layer


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("file", help="Input EDM ROOT file")
    parser.add_argument("--layer", required=True, type=layer_argument,
                        help="Layer encoded in HGCSiliconDetId (local to EE or HE silicon), or all")
    parser.add_argument("--module", default="hltHgcalDigisDecompressed")
    parser.add_argument("--instance", default="EE", help="Product instance, e.g. EE or HEfront")
    parser.add_argument("--process", default="ROUNDTRIP")
    parser.add_argument("--zside", type=int, choices=(-1, 1), help="Default: both endcaps")
    parser.add_argument("--max-events", type=int, default=1, help="Default: 1; -1 for all")
    parser.add_argument("--limit", type=int, default=0, help="Maximum rows per event; 0 prints all")
    args = parser.parse_args()
    if args.max_events < -1 or args.limit < 0:
        parser.error("--max-events must be >= -1 and --limit must be >= 0")

    import ROOT
    from DataFormats.FWLite import Events, Handle

    ROOT.gInterpreter.Declare('#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"')
    handle = Handle("edm::SortedCollection<HGCDataFrame<DetId,io_v1::HGCSample> >")
    label = (args.module, args.instance, args.process)
    print("run lumi event rawId rawId_hex layer zside waferU waferV cellU cellV")
    for index, event in enumerate(Events(args.file)):
        if args.max_events >= 0 and index >= args.max_events:
            break
        event.getByLabel(label, handle)
        if not handle.isValid():
            raise RuntimeError("Product not found: " + ":".join(label))
        eid = event.eventAuxiliary().id()
        count = 0
        for digi in handle.product():
            generic = digi.id()
            if generic.det() not in (ROOT.DetId.HGCalEE, ROOT.DetId.HGCalHSi):
                raise RuntimeError("This script supports silicon EE/HE digis only")
            detid = ROOT.HGCSiliconDetId(generic.rawId())
            if args.layer != "all" and detid.layer() != args.layer:
                continue
            if args.zside is not None and detid.zside() != args.zside:
                continue
            raw = int(detid.rawId())
            print(eid.run(), eid.luminosityBlock(), eid.event(), raw, f"0x{raw:08x}",
                  detid.layer(), detid.zside(), detid.waferU(), detid.waferV(),
                  detid.cellU(), detid.cellV())
            count += 1
            if args.limit and count >= args.limit:
                break
        print(f"Event {eid.run()}:{eid.luminosityBlock()}:{eid.event()}: "
              f"printed {count} digis for layer {args.layer}"
              + (f" (limit {args.limit})" if args.limit else ""), file=sys.stderr)


if __name__ == "__main__":
    main()
