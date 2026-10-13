#!/usr/bin/env python3
import argparse
from pathlib import Path

import ROOT


def main():
    parser = argparse.ArgumentParser(description="Render HGCal digi correlation histograms")
    parser.add_argument("input", nargs="?", default="hgcalDigiCorrelations.root")
    parser.add_argument("--output-dir", default="hgcalDigiCorrelationPlots")
    args = parser.parse_args()

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetNumberContours(100)
    output = Path(args.output_dir)
    output.mkdir(parents=True, exist_ok=True)

    source = ROOT.TFile.Open(args.input)
    if not source or source.IsZombie():
        raise RuntimeError(f"Cannot open {args.input}")
    directory = source.Get("ntuplizer")
    if not directory:
        raise RuntimeError("Missing TFileService directory 'ntuplizer'")

    histograms = sorted(
        (key.ReadObj() for key in directory.GetListOfKeys() if key.GetClassName().startswith("TH2")),
        key=lambda hist: hist.GetName(),
    )
    canvas = ROOT.TCanvas("canvas", "canvas", 900, 760)
    canvas.SetRightMargin(0.15)
    canvas.SetLogz(True)
    pdf = output / "all_correlations.pdf"
    canvas.Print(f"{pdf}[")
    for hist in histograms:
        hist.SetMinimum(1)
        hist.Draw("COLZ")
        canvas.Print(str(output / f"{hist.GetName()}.png"))
        canvas.Print(str(pdf))
    canvas.Print(f"{pdf}]")
    print(f"Wrote {len(histograms)} PNG files and {pdf}")


if __name__ == "__main__":
    main()
