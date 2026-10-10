"""Rebuild recHits from persisted SoA and sidecars and compare with the saved originals."""

import os
import runpy
from pathlib import Path

import FWCore.ParameterSet.Config as cms
from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.Eras.Era_Phase2C26I13M9_cff import Phase2C26I13M9

roundtrip = runpy.run_path(
    str(Path(__file__).with_name("hltHgcalDigisRoundTrip_cfg.py"))
)["process"]

process = cms.Process("READBACK", Phase2C26I13M9)
process.load("Configuration.Geometry.GeometryExtendedRun4D128Reco_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T35", "")
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        os.environ.get("HGCAL_ROUNDTRIP_FILE", "file:hltHgcalDigisRecHitRoundTripLZMA4.root")
    ),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.hltHgcalDigisDecompressedReadback = cms.EDProducer(
    "HGCalDigisFromRecHitSoA",
    src=cms.InputTag("hltHgcalDigisSoA", "", "ROUNDTRIP"),
)
process.hltHGCalUncalibRecHitReadback = roundtrip.hltHGCalUncalibRecHit.clone(
    HGCEEdigiCollection=cms.InputTag("hltHgcalDigisDecompressedReadback", "EE"),
    HGCHEFdigiCollection=cms.InputTag("hltHgcalDigisDecompressedReadback", "HEfront"),
    HGCHEBdigiCollection=cms.InputTag("hltHgcalDigisDecompressedReadback", "HEback"),
)
process.hgcalUncalibRecHitRoundTripValidator = cms.EDAnalyzer(
    "HGCalUncalibRecHitRoundTripValidator",
    original=cms.InputTag("hltHGCalUncalibRecHit", "", "ROUNDTRIP"),
    restored=cms.InputTag("hltHGCalUncalibRecHitReadback"),
)
process.readback = cms.Path(
    process.hltHgcalDigisDecompressedReadback
    + process.hltHGCalUncalibRecHitReadback
    + process.hgcalUncalibRecHitRoundTripValidator
)
process.options.wantSummary = True
