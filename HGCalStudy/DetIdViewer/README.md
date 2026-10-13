# HGCalStudy/DetIdViewer

Tools to inspect HGCal silicon digis (`HGCalDigiCollection`) for the Phase-2 **D128** geometry:

* a **2D viewer**: one self-contained HTML page with every digi of one event drawn as a hexagonal cell at its
  global (x, y) position, per layer and endcap, coloured by ADC, with DetId lookup;
* a **3D viewer**: one self-contained HTML page with every digi in 3D, one panel per endcap (+z, −z), coloured
  by estimated charge, with selection in |z|, r, ADC, TOT and TOA;
* a **digi ntuplizer** and correlation plots;
* a FWLite **DetId dump**.

The HTML pages embed all data (gzip + base64) and work offline in any recent browser (WebGL is needed for 3D).

Developed and tested in `CMSSW_20_0_0_patch1` (el9_amd64_gcc13), on
`hltHgcalDigisDecompressed:EE:ROUNDTRIP` from `hltHgcalDigisRecHitRoundTripZSTD3.root`.

## Setup

```bash
cmsrel CMSSW_20_0_0_patch1 && cd CMSSW_20_0_0_patch1/src && cmsenv
git cms-init
git cms-checkout-topic silviodonato:HGCalDigiViewer_20_0_X
scram b -j 8
```

All commands below are run from `$CMSSW_BASE/src` (outputs are written to the current directory);
`T=HGCalStudy/DetIdViewer/test`.

## Contents

| File | Purpose |
|---|---|
| `plugins/HGCalDigiGeometryExport.cc` | `HGCalDigiGeometryExport` EDAnalyzer: writes one event's digis as JSON (DetId fields, COG position, display polygon, samples). |
| `plugins/HGCalDigiNtuplizer.cc` | `HGCalDigiNtuplizer` EDAnalyzer: digi TTree and correlation TH2s via `TFileService`. |
| `test/exportHGCalDigiGeometry_cfg.py` | cmsRun config for the JSON export. |
| `test/plotHGCalDigiDetIds.py` | Builds the 2D viewer (and optional per-layer PNGs) from the export; runs the export itself if no JSON is given. |
| `test/plotHGCalDigi3D.py` | Builds the 3D viewer from the export. |
| `test/hgcalDetIdViewer.html`, `test/hgcalDigi3DViewer.html` | HTML/JS templates for the two viewers. |
| `test/validateHGCalPolygons.py` | Checks the exported polygons: regular hexagons, no interior overlaps; prints sample statistics. |
| `test/checkDetIdViewer.cjs`, `test/check3DViewer.cjs` | Node smoke tests for the generated pages (data decode, selectors, filters) with stub DOM/canvas/WebGL. |
| `test/hgcalDigiNtuple_cfg.py`, `test/plotHGCalDigiCorrelations.py` | Ntuple production and PNG/PDF rendering of its TH2s. |
| `test/dumpHGCalDigiDetIds.py` | FWLite text dump of digi DetIds for one layer. |

## Usage

### 1. Export one event

```bash
cmsRun $T/exportHGCalDigiGeometry_cfg.py inputFiles=file:hltHgcalDigisRecHitRoundTripZSTD3.root \
       module=hltHgcalDigisDecompressed instance=EE processName=ROUNDTRIP jsonOutput=hgcal-d128-cells.json
python3 $T/validateHGCalPolygons.py hgcal-d128-cells.json
```

`instance=HEfront` selects the HE silicon collection. Only the first event is exported.

JSON schema 2: `cells` is a list of
`[rawId, layer, zside, waferU, waferV, cellU, cellV, x, y, z, polygon, samples]`, with `x, y, z` the
reconstructed cell centre of gravity in global cm (`HGCalGeometry::getPosition`), `polygon` six `[x, y]`
vertices and `samples` one `[data, mode, toa]` per bunch crossing (`mode` = 1 for TOT, `data` = raw ADC or TOT
word, `toa` = raw TOA word). Exports made before TOA was added have `[data, mode]`; both viewers accept them.

### 2. 2D viewer

```bash
python3 $T/plotHGCalDigiDetIds.py hltHgcalDigisRecHitRoundTripZSTD3.root --layer all --zside all \
        --geometry-json hgcal-d128-cells.json --output hgcal-d128-all-detids --no-png
node $T/checkDetIdViewer.cjs hgcal-d128-all-detids.html
```

Produces `hgcal-d128-all-detids.html`. Drop `--no-png` to also write one PNG per layer/endcap; use e.g.
`--layer 10 --zside 1` for a single slice. Without `--geometry-json` the script runs the export first.

In the page: choose layer, endcap, wafer and sample (2 = in-time); scroll to zoom, drag to pan, hover/click a
cell for its DetId, ADC/TOT value and position; find a cell by decimal or hex DetId.

### 3. 3D viewer

```bash
python3 $T/plotHGCalDigi3D.py --geometry-json hgcal-d128-cells.json --output hgcal-d128-3d.html
node $T/check3DViewer.cjs hgcal-d128-3d.html
```

Produces `hgcal-d128-3d.html` (≈3 MB for 392k digis). In the page:

* two panels, +z and −z; drag to rotate, shift-/right-drag to pan, scroll to zoom, double-click to reset;
  by default the cameras are linked and mirrored, so both endcaps are seen from the interaction-point side;
* **selection**: |z| and r ranges [cm]; ADC, TOT and TOA ranges (raw words of the selected sample; ADC and TOT
  ranges apply only to digis in that mode); ADC-mode / TOT-mode toggles; the sample selector;
* **colour** (log scale) and optionally point size encode an estimated charge
  `Q = ADC × 0.0977 fC` or `Q = TOT × 2.44 fC` (nominal 100 fC / 10-bit ADC and 10 pC / 12-bit TOT LSBs,
  editable in the page). This is **not** a calibrated energy: no pedestal, per-channel gain, TOT
  non-linearity, sensor-thickness or MIP/MeV conversion is applied;
* click a point for its DetId, position and all samples; per-endcap counts and ΣQ are shown in each panel.

### 4. Ntuple and correlation plots

```bash
cmsRun $T/hgcalDigiNtuple_cfg.py maxEvents=50 outputFile=hgcalDigiCorrelations.root
python3 $T/plotHGCalDigiCorrelations.py hgcalDigiCorrelations.root --output-dir hgcalDigiCorrelationPlots
```

### 5. DetId dump (FWLite)

```bash
python3 $T/dumpHGCalDigiDetIds.py hltHgcalDigisRecHitRoundTripZSTD3.root --layer 1 --zside 1 --limit 20
```

## Display polygons

`HGCalDigiGeometryExport` draws each cell as a regular hexagon with corners at k·60° in global x–y,
centred on the nominal cell position `getPosition(id, false, false)`, with circumradius
`kDisplayScale × waferSize / (3N)` (N = 12 for HD120/HD200, 8 otherwise).

* The corner directions are **not** taken from `HGCalGeometry::getCorners()`: on the −z endcap its corners are
  not centred on the nominal position, and the earlier normalised-direction construction collapsed −z cells
  into slivers. The fixed k·60° orientation matches every correctly drawn cell on both endcaps.
* HD200 (type 3) cells must use N = 12; `cellSizeHex(type=3)` in this release picks the coarse size.

**Known limitation (open):** with `kDisplayScale = 1.0` (true cell size) cells tile the wafer without gaps,
but cells on the wafer boundary are physically partial (cut) hexagons whose nominal centre is shifted inward,
so the full hexagons drawn for them overlap their neighbours by up to ≈1 mm. `validateHGCalPolygons.py`
therefore reports ≈53k overlapping pairs on the reference event (all involving edge cells) and exits non-zero.
Options: accept edge overlaps, clip edge cells to the wafer outline (exact), or use a display scale ≈0.85–0.9.
The positions (COG) printed and used for picking are unaffected.
