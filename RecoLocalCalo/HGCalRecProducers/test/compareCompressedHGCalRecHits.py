#!/usr/bin/env python3
"""Compare original and decompressed HGCal uncalibrated RecHits with FWLite.

Reads the two HGCUncalibratedRecHitCollection flavours written by
stepHLT_onlyHGCalCompression.py (hltHGCalUncalibRecHit vs
hltHGCalUncalibRecHitDecompressed), matches the hits by DetId and books, for
every hit variable, the original and decompressed spectra, their residual
(decompressed - original), the 2D correlation and the residual profiles versus
amplitude and versus layer.

Example:
    cmsenv
    ./compareCompressedHGCalRecHits.py stepHLT_onlyHGCalCompression.root \\
        -o compressionComparison.root -n 10

Everything is also summarised as a text table on stdout, so the script is
useful even without looking at the histograms.
"""

import argparse
import sys
from collections import OrderedDict

import numpy as np
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gSystem.Load("libFWCoreFWLite.so")
ROOT.FWLiteEnabler.enable()

from DataFormats.FWLite import Events, Handle  # noqa: E402  (needs FWLite enabled first)

UNCALIB_TYPE = ("edm::SortedCollection<HGCUncalibratedRecHit,"
                "edm::StrictWeakOrdering<HGCUncalibratedRecHit> >")

# (branch alias, python-side getter name) for the float payload of a hit.
FLOAT_VARS = OrderedDict([
    ("amplitude", "amplitude"),
    ("pedestal", "pedestal"),
    ("jitter", "jitter"),
    ("chi2", "chi2"),
    ("outOfTimeEnergy", "outOfTimeEnergy"),
    ("outOfTimeChi2", "outOfTimeChi2"),
    ("jitterError", "jitterError"),
])

# Integer payload, compared exactly rather than histogrammed as a residual.
INT_VARS = OrderedDict([
    ("flags", "flags"),
    ("aux", "aux"),
])

# DetId bit layout, from DataFormats/DetId/interface/DetId.h and
# DataFormats/ForwardDetId/interface/HGC{Silicon,Scintillator}DetId.h.
DET_OFFSET, DET_MASK = 28, 0xF
DET_HGCAL_EE, DET_HGCAL_HSI, DET_HGCAL_HSC = 8, 9, 10
SI_LAYER_OFFSET, SI_LAYER_MASK = 20, 0x1F
SC_LAYER_OFFSET, SC_LAYER_MASK = 17, 0x1F
ZSIDE_OFFSET, ZSIDE_MASK = 25, 0x1


def layers_from_ids(raw_ids):
    """Decode the HGCal layer number for an array of raw DetIds."""
    det = (raw_ids >> DET_OFFSET) & DET_MASK
    layer = np.zeros(raw_ids.shape, dtype=np.int32)
    silicon = (det == DET_HGCAL_EE) | (det == DET_HGCAL_HSI)
    scintillator = det == DET_HGCAL_HSC
    layer[silicon] = (raw_ids[silicon] >> SI_LAYER_OFFSET) & SI_LAYER_MASK
    layer[scintillator] = (raw_ids[scintillator] >> SC_LAYER_OFFSET) & SC_LAYER_MASK
    return layer


def zsides_from_ids(raw_ids):
    return np.where((raw_ids >> ZSIDE_OFFSET) & ZSIDE_MASK, -1, 1).astype(np.int32)


READER_NAME = "hgcalUncalibRecHitsToArrays"


def reader_source():
    """Generate the C++ extraction loop from FLOAT_VARS / INT_VARS.

    Generated rather than hand-written so the C++ column order cannot drift
    away from the Python variable lists that index the same buffers.
    """
    floats = "\n".join(f"      floats[{ivar} * n + ihit] = hit.{getter}();"
                       for ivar, getter in enumerate(FLOAT_VARS.values()))
    ints = "\n".join(f"      ints[{ivar} * n + ihit] = hit.{getter}();"
                     for ivar, getter in enumerate(INT_VARS.values()))
    return f"""
    #include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
    #include <cstdint>
    #include <cstddef>

    void {READER_NAME}(const HGCUncalibratedRecHitCollection& hits,
                       uint32_t* ids, double* floats, int64_t* ints) {{
      const std::size_t n = hits.size();
      for (std::size_t ihit = 0; ihit < n; ++ihit) {{
        const HGCUncalibratedRecHit& hit = hits[ihit];
        ids[ihit] = hit.id().rawId();
{floats}
{ints}
      }}
    }}
    """


def declare_reader():
    """JIT-compile the extraction loop; return the callable, or None on failure."""
    if not ROOT.gInterpreter.Declare(reader_source()):
        return None
    return getattr(ROOT, READER_NAME)


def read_collection(event, handle, label, reader):
    """Return (raw_ids, float_values, int_values) as numpy arrays for one collection.

    float_values has shape (len(FLOAT_VARS), nhits), int_values (len(INT_VARS), nhits).
    Returns None when the collection is absent from the event.
    """
    if not event.getByLabel(label, handle):
        return None
    hits = handle.product()
    nhits = hits.size()

    raw_ids = np.empty(nhits, dtype=np.uint32)
    floats = np.empty((len(FLOAT_VARS), nhits), dtype=np.float64)
    ints = np.empty((len(INT_VARS), nhits), dtype=np.int64)
    if nhits == 0:
        return raw_ids, floats, ints

    if reader is not None:
        # One Python->C++ crossing for the whole collection; the loop below
        # costs one crossing per hit *per variable*, which dominates everything
        # else in the job.
        reader(hits, raw_ids, floats, ints)
        return raw_ids, floats, ints

    float_getters = [getattr(ROOT.HGCUncalibratedRecHit, getter) for getter in FLOAT_VARS.values()]
    int_getters = [getattr(ROOT.HGCUncalibratedRecHit, getter) for getter in INT_VARS.values()]
    for ihit in range(nhits):
        hit = hits[ihit]
        raw_ids[ihit] = hit.id().rawId()
        for ivar, getter in enumerate(float_getters):
            floats[ivar, ihit] = getter(hit)
        for ivar, getter in enumerate(int_getters):
            ints[ivar, ihit] = getter(hit)
    return raw_ids, floats, ints


def axis_range(values, pad=0.05):
    """Robust histogram range: 0.1-99.9 percentiles, padded, never degenerate."""
    if values.size == 0:
        return 0.0, 1.0
    lo, hi = np.percentile(values, [0.1, 99.9])
    if not np.isfinite(lo) or not np.isfinite(hi):
        return 0.0, 1.0
    if hi <= lo:
        span = max(abs(hi), 1.0) * 0.5
        return lo - span, hi + span
    span = hi - lo
    return lo - pad * span, hi + pad * span


def symmetric_range(residuals, fallback=1e-6):
    """Symmetric range around zero for a residual axis."""
    if residuals.size == 0:
        return fallback
    edge = np.percentile(np.abs(residuals), 99.9)
    if not np.isfinite(edge) or edge <= 0.0:
        # Lossless so far: keep a narrow window so any future deviation shows up.
        return fallback
    return 1.2 * edge


def booking_ranges(original, decompressed):
    """Derive the histogram axis ranges for every variable from one event.

    The value axes follow the original spectrum, the residual axes follow the
    residual of the DetId-matched hits.
    """
    ids_o, floats_o, _ = original
    ids_d, floats_d, _ = decompressed
    _, idx_o, idx_d = np.intersect1d(ids_o, ids_d, assume_unique=True, return_indices=True)

    ranges = {}
    for ivar, var in enumerate(FLOAT_VARS):
        residual = floats_d[ivar, idx_d] - floats_o[ivar, idx_o]
        ranges[var] = {
            "value": axis_range(floats_o[ivar]),
            "residual": symmetric_range(residual),
        }
    return ranges


def fill_1d(hist, values):
    if values.size:
        hist.FillN(values.size, values, np.ones(values.size))


def fill_2d(hist, xvalues, yvalues):
    if xvalues.size:
        hist.FillN(xvalues.size, xvalues, yvalues, np.ones(xvalues.size), 1)


class CollectionPlots:
    """Histograms and running statistics for one (original, decompressed) pair."""

    def __init__(self, name, outfile, nbins, amplitude_range, ranges):
        self.name = name
        self.directory = outfile.mkdir(name)
        self.directory.cd()
        self.nbins = nbins

        self.n_events = 0
        self.n_original = 0
        self.n_decompressed = 0
        self.n_matched = 0
        self.n_missing = 0    # in original, absent from decompressed
        self.n_spurious = 0   # in decompressed, absent from original
        # Per-variable running stats on the residual: count, sum, sum of squares, max|.|
        self.stats = {var: [0, 0.0, 0.0, 0.0] for var in FLOAT_VARS}
        self.n_identical = dict.fromkeys(FLOAT_VARS, 0)
        self.n_int_mismatch = dict.fromkeys(INT_VARS, 0)

        self.h_nhits_original = ROOT.TH1F(
            "nhits_original", f"{name};hits / event (original);events", 100, 0, 0)
        self.h_nhits_decompressed = ROOT.TH1F(
            "nhits_decompressed", f"{name};hits / event (decompressed);events", 100, 0, 0)
        for hist in (self.h_nhits_original, self.h_nhits_decompressed):
            hist.SetBuffer(1000)

        self.h_layer_original = ROOT.TH1F(
            "layer_original", f"{name};layer;hits (original)", 50, 0.5, 50.5)
        self.h_layer_matched = ROOT.TH1F(
            "layer_matched", f"{name};layer;matched hits", 50, 0.5, 50.5)
        self.h_zside = ROOT.TH1F("zside", f"{name};z side;hits", 3, -1.5, 1.5)

        self.original = {}
        self.decompressed = {}
        self.residual = {}
        self.relative = {}
        self.correlation = {}
        self.residual_vs_amplitude = {}
        self.residual_vs_layer = {}

        for var in FLOAT_VARS:
            lo, hi = ranges[var]["value"]
            edge = ranges[var]["residual"]
            self.original[var] = ROOT.TH1F(
                f"{var}_original", f"{name};{var} (original);hits", nbins, lo, hi)
            self.decompressed[var] = ROOT.TH1F(
                f"{var}_decompressed", f"{name};{var} (decompressed);hits", nbins, lo, hi)
            self.residual[var] = ROOT.TH1F(
                f"{var}_residual",
                f"{name};{var}: decompressed - original;hits", nbins, -edge, edge)
            self.relative[var] = ROOT.TH1F(
                f"{var}_relativeResidual",
                f"{name};{var}: (decompressed - original) / original;hits",
                nbins, -0.2, 0.2)
            self.correlation[var] = ROOT.TH2F(
                f"{var}_correlation",
                f"{name};{var} (original);{var} (decompressed)",
                200, lo, hi, 200, lo, hi)
            self.residual_vs_amplitude[var] = ROOT.TProfile(
                f"{var}_residualVsAmplitude",
                f"{name};amplitude (original);#LT{var} residual#GT",
                100, amplitude_range[0], amplitude_range[1], "s")
            self.residual_vs_layer[var] = ROOT.TProfile(
                f"{var}_residualVsLayer",
                f"{name};layer;#LT|{var} residual|#GT", 50, 0.5, 50.5, "s")

    def fill(self, original, decompressed):
        ids_o, floats_o, ints_o = original
        ids_d, floats_d, ints_d = decompressed

        self.n_events += 1
        self.n_original += ids_o.size
        self.n_decompressed += ids_d.size
        self.h_nhits_original.Fill(ids_o.size)
        self.h_nhits_decompressed.Fill(ids_d.size)

        layers_o = layers_from_ids(ids_o)
        fill_1d(self.h_layer_original, layers_o.astype(np.float64))
        fill_1d(self.h_zside, zsides_from_ids(ids_o).astype(np.float64))

        _, idx_o, idx_d = np.intersect1d(ids_o, ids_d, assume_unique=True, return_indices=True)
        self.n_matched += idx_o.size
        self.n_missing += ids_o.size - idx_o.size
        self.n_spurious += ids_d.size - idx_d.size
        if idx_o.size == 0:
            return

        layers = layers_o[idx_o].astype(np.float64)
        fill_1d(self.h_layer_matched, layers)
        amplitudes = floats_o[list(FLOAT_VARS).index("amplitude"), idx_o]

        for ivar, var in enumerate(FLOAT_VARS):
            values_o = floats_o[ivar, idx_o]
            values_d = floats_d[ivar, idx_d]
            residual = values_d - values_o

            fill_1d(self.original[var], values_o)
            fill_1d(self.decompressed[var], values_d)
            fill_1d(self.residual[var], residual)
            fill_2d(self.correlation[var], values_o, values_d)
            fill_2d(self.residual_vs_amplitude[var], amplitudes, residual)
            fill_2d(self.residual_vs_layer[var], layers, np.abs(residual))

            nonzero = values_o != 0.0
            if nonzero.any():
                fill_1d(self.relative[var], residual[nonzero] / values_o[nonzero])

            stats = self.stats[var]
            stats[0] += residual.size
            stats[1] += float(residual.sum())
            stats[2] += float(np.dot(residual, residual))
            stats[3] = max(stats[3], float(np.abs(residual).max()))
            self.n_identical[var] += int((residual == 0.0).sum())

        for ivar, var in enumerate(INT_VARS):
            self.n_int_mismatch[var] += int((ints_d[ivar, idx_d] != ints_o[ivar, idx_o]).sum())

    def summary_rows(self):
        """One row per float variable: mean, RMS and max of the residual."""
        rows = []
        for var in FLOAT_VARS:
            count, total, total2, maximum = self.stats[var]
            if count == 0:
                rows.append((var, float("nan"), float("nan"), float("nan"), float("nan")))
                continue
            mean = total / count
            variance = max(total2 / count - mean * mean, 0.0)
            identical = 100.0 * self.n_identical[var] / count
            rows.append((var, mean, variance ** 0.5, maximum, identical))
        return rows

    def print_summary(self):
        print(f"\n=== {self.name} ===")
        print(f"  events                 : {self.n_events}")
        print(f"  hits original          : {self.n_original}")
        print(f"  hits decompressed      : {self.n_decompressed}")
        print(f"  hits matched by DetId  : {self.n_matched}")
        print(f"  only in original       : {self.n_missing}")
        print(f"  only in decompressed   : {self.n_spurious}")
        for var, mismatches in self.n_int_mismatch.items():
            print(f"  {var:<21}: {mismatches} mismatches")
        if self.n_matched == 0:
            return
        print(f"  {'variable':<18} {'<residual>':>13} {'RMS':>13} "
              f"{'max|residual|':>13} {'identical [%]':>14}")
        for var, mean, rms, maximum, identical in self.summary_rows():
            print(f"  {var:<18} {mean:>13.6g} {rms:>13.6g} {maximum:>13.6g} {identical:>14.4f}")

    def write(self):
        self.directory.cd()
        self.directory.Write()


def draw_overview(plots, pdf_path):
    """Overlay original/decompressed spectra and residuals into a single PDF."""
    ROOT.gStyle.SetOptStat(0)
    # Colourblind-safe pair for the two series.
    colour_original, colour_decompressed = ROOT.TColor.GetColor("#3b6ea5"), ROOT.TColor.GetColor("#d1731f")

    canvas = ROOT.TCanvas("overview", "overview", 1400, 900)
    canvas.Print(pdf_path + "[")
    for entry in plots:
        canvas.Clear()
        canvas.Divide(4, 2)
        for ipad, var in enumerate(FLOAT_VARS, start=1):
            pad = canvas.cd(ipad)
            pad.SetLogy()
            original, decompressed = entry.original[var], entry.decompressed[var]
            original.SetLineColor(colour_original)
            original.SetLineWidth(2)
            decompressed.SetLineColor(colour_decompressed)
            decompressed.SetLineStyle(2)
            decompressed.SetLineWidth(2)
            original.Draw("hist")
            decompressed.Draw("hist same")
            if ipad == 1:
                legend = ROOT.TLegend(0.55, 0.75, 0.88, 0.88)
                legend.SetBorderSize(0)
                legend.AddEntry(original, "original", "l")
                legend.AddEntry(decompressed, "decompressed", "l")
                legend.Draw()
                entry._legend = legend  # keep alive until the pad is printed
        pad = canvas.cd(8)
        pad.SetLogy()
        entry.residual["amplitude"].SetLineColor(colour_decompressed)
        entry.residual["amplitude"].SetLineWidth(2)
        entry.residual["amplitude"].Draw("hist")
        canvas.Print(pdf_path)
    canvas.Print(pdf_path + "]")
    print(f"\nWrote {pdf_path}")


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("files", nargs="+", help="input EDM ROOT file(s)")
    parser.add_argument("-o", "--output", default="compressionComparison.root",
                        help="output ROOT file with the histograms")
    parser.add_argument("-n", "--max-events", type=int, default=-1,
                        help="number of events to process (-1 = all)")
    parser.add_argument("--original", default="hltHGCalUncalibRecHit",
                        help="module label of the original collections")
    parser.add_argument("--decompressed", default="hltHGCalUncalibRecHitDecompressed",
                        help="module label of the decompressed collections")
    parser.add_argument("--process", default="",
                        help="process name of both collections (empty = any)")
    parser.add_argument("--instances", nargs="+",
                        default=["HGCEEUncalibRecHits", "HGCHEFUncalibRecHits",
                                 "HGCHEBUncalibRecHits"],
                        help="product instance names to compare")
    parser.add_argument("--bins", type=int, default=200, help="bins per 1D histogram")
    parser.add_argument("--pdf", default="",
                        help="also write an overview PDF to this path")
    parser.add_argument("--python-loop", action="store_true",
                        help="extract the hits in Python instead of JIT-compiled C++ "
                             "(orders of magnitude slower; for cross-checking)")
    parser.add_argument("--imt", type=int, default=0, metavar="NTHREADS",
                        help="enable ROOT implicit multi-threading with this many threads "
                             "(0 = off, -1 = as many as cores); parallelises basket "
                             "decompression inside TTree reading")
    args = parser.parse_args()

    if args.imt:
        # Must happen before any file is opened.
        ROOT.EnableImplicitMT(args.imt if args.imt > 0 else 0)
        print(f"[info] implicit MT enabled with {ROOT.GetThreadPoolSize()} threads")

    reader = None
    if args.python_loop:
        print("[info] using the Python extraction loop on request")
    else:
        reader = declare_reader()
        if reader is None:
            print("[warning] could not JIT-compile the extraction loop, "
                  "falling back to Python (slow)", file=sys.stderr)

    events = Events(args.files)
    handle_original = Handle(UNCALIB_TYPE)
    handle_decompressed = Handle(UNCALIB_TYPE)

    outfile = ROOT.TFile.Open(args.output, "recreate")
    plots = OrderedDict()

    n_processed = 0
    n_absent = dict.fromkeys(args.instances, 0)
    for event in events:
        if args.max_events >= 0 and n_processed >= args.max_events:
            break
        n_processed += 1

        for instance in args.instances:
            label_original = (args.original, instance, args.process)
            label_decompressed = (args.decompressed, instance, args.process)
            original = read_collection(event, handle_original, label_original, reader)
            decompressed = read_collection(event, handle_decompressed, label_decompressed, reader)
            if original is None or decompressed is None:
                # Normal for events rejected by the filters upstream of the
                # compressor: the products are simply not there.
                n_absent[instance] += 1
                continue

            if instance not in plots:
                # Book on the first event so the axes follow the actual data range.
                ranges = booking_ranges(original, decompressed)
                plots[instance] = CollectionPlots(
                    instance, outfile, args.bins,
                    ranges["amplitude"]["value"], ranges)

            plots[instance].fill(original, decompressed)

        print(f"processed event {n_processed}", flush=True)

    if not plots:
        print(f"[error] none of {args.instances} was found in {n_processed} events; "
              f"check --original / --decompressed / --process", file=sys.stderr)
        return 1

    for instance, entry in plots.items():
        entry.print_summary()
        if n_absent[instance]:
            print(f"  events without the products: {n_absent[instance]} "
                  f"(rejected upstream of the compressor)")
        entry.write()

    if args.pdf:
        draw_overview(list(plots.values()), args.pdf)

    outfile.Close()
    print(f"\nWrote {args.output} ({n_processed} events)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
