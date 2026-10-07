# Digi collections to HGCalDigiSoA

## Lossless `hltHgcalDigis` round trip

`HGCalDigisToLosslessSoA` converts the `EE`, `HEfront`, and `HEback` instances
of `hltHgcalDigis` to the standard `hgcaldigi::HGCalDigiHost` format used by
`hgcalDigis` in raw-data reconstruction. Each SoA row contains the in-time
sample projection. Seven sidecar vectors per instance retain the information
that the standard SoA cannot hold: the DetId, sample count, packed raw sample
bits, and rare full-word exceptions. `HGCalDigisFromLosslessSoA` uses the SoA
and all seven sidecars to restore every original 32-bit sample word and DetId.
Frames with more than five samples cause a clear exception instead of data
loss.

The product type matches the raw-data SoA, but its rows follow the sorted
legacy DetId order and are marked `Invalid`. Reconstruction with the raw-data
SoA needs its channel index mapping and calibrated ADC/ToT scales.

The single `hltHgcalDigisSoA` producer reads all three instances and emits
`EE`, `HEfront`, and `HEback` SoAs. Each has `DetIdDeltas`, `SampleCounts`,
`SharedBits`, `StatusBits`, `DataBits`, `Exceptions`, and `DetIdExceptions`
sidecars with the same instance prefix. The single
`hltHgcalDigisDecompressed` producer reads all 24 products and emits the
three restored digi instances. One validator checks all three pairs.

The round-trip example reads `onlyHGCalSimDigisZSTD3.root`, which contains
`simHGCalUnsuppressedDigis` but no `hltHgcalDigis`. The `HGCalRawToDigiFake`
producer first creates the three `hltHgcalDigis` instances from the EE,
HEfront, and HEback sim digis.

The separate `HGCalDataFrameToHGCalDigiSoA` converter below writes the same
standard SoA without these lossless sidecars, so it cannot reproduce the
original five-sample frame.

From the CMSSW area, after `cmsenv` and `scram b -j 4`, run:

```sh
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/hltHgcalDigisRoundTrip_cfg.py
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/hltHgcalDigisReadback_cfg.py
python3 src/RecoLocalCalo/HGCalDigiSoAConverter/test/reportRoundTripSizes.py hltHgcalDigisRoundTripLZMA4.root
```

The first job writes `hltHgcalDigisRoundTripLZMA4.root` and
`hltHgcalDigisRoundTripZSTD3.root` with the original, lossless SoA, and
restored branches for all three instances. Its validator compares frame counts,
DetIds, sample counts, and every raw sample word in every event. The second
job reads the persisted SoA and sidecars from the ROOT file and repeats the
comparison. The output modules use LZMA level 4 and ZSTD level 3 with split
level 0. The size script reports compressed and uncompressed kB per event for
each instance and for the combined collection.

## In-time sample projection

`HGCalDataFrameToHGCalDigiSoA` reads the legacy `HGCalDigiCollection`,
including `simHGCalUnsuppressedDigis:EE:HLT`. It emits a
`hgcaldigi::HGCalDigiHost` and a row-aligned `detIds` vector. Each input
frame produces one row. The default sample index is 2, the in-time sample
used by legacy HGCal rec-hit reconstruction. Set `sampleIndex` to select a
different sample. An absent input collection produces empty outputs.

The converter copies ADC-mode data to `adc`, ToT-mode data to `tot`, and a
valid ToA to `toa`. It copies the preceding ADC-mode sample to `adcm1`.
It uses `tctp=2` for complete ToT and `tctp=1` for busy/in-progress ToT;
other rows use zero. `cm` is zero because the legacy frame has no common-mode
field. It marks all rows `hgcal::DIGI_FLAG::Invalid`: legacy ADC/ToT scales
and channel indexing differ from the raw-data SoA, so these are not valid
inputs to the newer HGCal reconstruction without further calibration and
mapping.

Run the HGCal example from the CMSSW area after `cmsenv` and `scram b -j 4`:

```sh
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/hgcalDataFrameToHGCalDigiSoA_cfg.py
```

It reads one event from the supplied file and writes
`hgcalDataFrameToHGCalDigiSoA.root`.

## QIE10 input

`QIE10ToHGCalDigiSoA` reads a `QIE10DigiCollection` (the
`HcalDataFrameContainer<io_v1::QIE10DataFrame>` product) and emits a
`hgcaldigi::HGCalDigiHost`, the persistable host collection for
`HGCalDigiSoA`. It also emits `std::vector<uint32_t>` with instance name
`detIds`, in the same row order as the SoA.

The `hltHgcalDigis:EE` branch in the example file is a separate
`HGCalDigiCollection`; this producer consumes `hltHcalDigis` instead.

Each input frame produces one row. The producer selects the sample marked
SOI, falling back to sample zero if none is marked. It copies that sample's
ADC to `adc`, the preceding sample's ADC (or zero) to `adcm1`, and the raw
leading-edge TDC count to `toa`. It sets `tctp`, `tot`, and `cm` to zero and
`flags` to `hgcal::DIGI_FLAG::Invalid`. QIE10 and HGCal digitizers use
different encodings; this output is intended for studying the SoA layout and
is not a calibrated HGCal digi for reconstruction. The QIE10 trailing-edge
TDC, CAPID, and status bits are not represented in this layout.

From the CMSSW area, run:

```sh
cmsenv
scram b -j 4
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/qie10ToHGCalDigiSoA_cfg.py
```

The example reads two events from the supplied file and writes only the SoA
and its `detIds` product to `qie10ToHGCalDigiSoA.root`.
The first event has no QIE10 product and produces an empty SoA; the second
has 3,456 frames.
