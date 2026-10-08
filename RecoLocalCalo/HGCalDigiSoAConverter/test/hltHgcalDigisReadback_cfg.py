"""Rebuild recHits from persisted SoA/index deltas and compare with the saved originals."""

import os

import FWCore.ParameterSet.Config as cms
from Configuration.AlCa.GlobalTag import GlobalTag
from Configuration.Eras.Era_Phase2C26I13M9_cff import Phase2C26I13M9

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
process.hltHGCalUncalibRecHit = cms.EDProducer(
    "HGCalUncalibRecHitProducer",
    HGCEEConfig=cms.PSet(
        adcNbits=cms.uint32(10),
        adcSaturation=cms.double(100),
        fCPerMIP=cms.vdouble(2.06, 3.43, 5.15),
        isSiFE=cms.bool(True),
        tdcNbits=cms.uint32(12),
        tdcOnset=cms.double(60),
        tdcSaturation=cms.double(10000),
        toaLSB_ns=cms.double(0.0244),
        tofDelay=cms.double(-9),
    ),
    HGCEEdigiCollection=cms.InputTag("hltHgcalDigis", "EE"),
    HGCEEhitCollection=cms.string("HGCEEUncalibRecHits"),
    HGCHEBConfig=cms.PSet(
        adcNbits=cms.uint32(10),
        adcSaturation=cms.double(68.75),
        fCPerMIP=cms.vdouble(1.0, 1.0, 1.0),
        isSiFE=cms.bool(True),
        tdcNbits=cms.uint32(12),
        tdcOnset=cms.double(55),
        tdcSaturation=cms.double(1000),
        toaLSB_ns=cms.double(0.0244),
        tofDelay=cms.double(-14),
    ),
    HGCHEBdigiCollection=cms.InputTag("hltHgcalDigis", "HEback"),
    HGCHEBhitCollection=cms.string("HGCHEBUncalibRecHits"),
    HGCHEFConfig=cms.PSet(
        adcNbits=cms.uint32(10),
        adcSaturation=cms.double(100),
        fCPerMIP=cms.vdouble(2.06, 3.43, 5.15),
        isSiFE=cms.bool(True),
        tdcNbits=cms.uint32(12),
        tdcOnset=cms.double(60),
        tdcSaturation=cms.double(10000),
        toaLSB_ns=cms.double(0.0244),
        tofDelay=cms.double(-11),
    ),
    HGCHEFdigiCollection=cms.InputTag("hltHgcalDigis", "HEfront"),
    HGCHEFhitCollection=cms.string("HGCHEFUncalibRecHits"),
    HGCHFNoseConfig=cms.PSet(
        adcNbits=cms.uint32(10),
        adcSaturation=cms.double(100),
        fCPerMIP=cms.vdouble(1.25, 2.57, 3.88),
        isSiFE=cms.bool(False),
        tdcNbits=cms.uint32(12),
        tdcOnset=cms.double(60),
        tdcSaturation=cms.double(10000),
        toaLSB_ns=cms.double(0.0244),
        tofDelay=cms.double(-33),
    ),
    HGCHFNosedigiCollection=cms.InputTag("hfnoseDigis", "HFNose"),
    HGCHFNosehitCollection=cms.string("HGCHFNoseUncalibRecHits"),
    algo=cms.string("HGCalUncalibRecHitWorkerWeights"),
    computeLocalTime=cms.bool(True),
)
process.hltHGCalUncalibRecHitReadback = process.hltHGCalUncalibRecHit.clone(
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
