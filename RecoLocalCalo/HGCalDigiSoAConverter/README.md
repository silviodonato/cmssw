# Digi collections to HGCalDigiSoA

## RecHit-equivalent `hltHgcalDigis` conversion

`HGCalDigisToRecHitSoA` converts `EE`, `HEfront`, and `HEback` legacy digis to
the standard `hgcaldigi::HGCalDigiHost` type. It keeps the fields read by
`HGCalUncalibRecHitRecWeightsAlgo`: DetId and sample 2's data, mode,
threshold, ToA-valid bit, and ToA. It does **not** preserve the other four
samples or unused bits, so the decompressed digis are intentionally not
identical to the input digis.

One SoA row represents one input frame. The private legacy mapping uses `tctp`
bits 0, 1, and 2 for mode, threshold, and ToA-valid respectively; the 12-bit
in-time data goes to `adc` or `tot`, including busy TDC samples. `toa` holds
the valid in-time ToA. The rows follow sorted DetId order and have
`flags=Invalid`, because their channel indexing and ADC/ToT scales are not
those of native ECON-D digis. One `DetIds` sidecar per detector instance
stores a raw `uint32_t` DetId for every row. The standard SoA has no DetId column.
The decoder restores five-sample legacy frames with zero in the four unused
sample slots and the original recHit-relevant fields in sample 2. Frames with
fewer than three samples are rejected, because the recHit algorithm would
read beyond their end.

The example reads `onlyHGCalSimDigisZSTD3.root`. `HGCalRawToDigiFake` creates
`hltHgcalDigis` from its EE, HEfront, and HEback sim digis. Two identical
`HGCalUncalibRecHitProducer` configurations reconstruct from the original and
decompressed digis. `HGCalUncalibRecHitRoundTripValidator` compares all hit
fields and DetIds bit-for-bit in each event and fails on any mismatch.

From the CMSSW area, after `cmsenv` and `scram b -j 4`, run:

```sh
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/hltHgcalDigisRoundTrip_cfg.py
cmsRun src/RecoLocalCalo/HGCalDigiSoAConverter/test/hltHgcalDigisReadback_cfg.py
python3 src/RecoLocalCalo/HGCalDigiSoAConverter/test/reportRoundTripSizes.py hltHgcalDigisRecHitRoundTripLZMA4.root
```

The first job writes `hltHgcalDigisRecHitRoundTripLZMA4.root` and
`hltHgcalDigisRecHitRoundTripZSTD3.root` with the original digis, compact SoA,
decompressed digis, and both recHit collections. The second job rebuilds
recHits from the persisted SoA and DetIds and compares them with the originals.
The output modules use LZMA level 4 and ZSTD level 3 with
split level 0. The size script reports compressed and uncompressed kB per
event for the original digis, SoA with sidecars, and decompressed digis.

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
