import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Phase2C26I13M9_cff import Phase2C26I13M9
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("ROUNDTRIP", Phase2C26I13M9)
process.load("Configuration.Geometry.GeometryExtendedRun4D128Reco_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T35", "")

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "file:/shared/sdonato/HGCalSoADigiConverter/CMSSW_20_0_0_patch1/src/onlyHGCalSimDigisZSTD3.root"
    ),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.hltHgcalDigis = cms.EDProducer(
    "HGCalRawToDigiFake",
    eeDigis=cms.InputTag("simHGCalUnsuppressedDigis", "EE", "HLT"),
    fhDigis=cms.InputTag("simHGCalUnsuppressedDigis", "HEfront", "HLT"),
    bhDigis=cms.InputTag("simHGCalUnsuppressedDigis", "HEback", "HLT"),
)
process.hltHgcalDigisSoA = cms.EDProducer(
    "HGCalDigisToRecHitSoA",
    src=cms.InputTag("hltHgcalDigis"),
)
process.hltHgcalDigisDecompressed = cms.EDProducer(
    "HGCalDigisFromRecHitSoA",
    src=cms.InputTag("hltHgcalDigisSoA"),
)

# Both modules use the same reconstruction parameters; only the digi source differs.
process.hltHGCalUncalibRecHit = cms.EDProducer(
    "HGCalUncalibRecHitProducer",
    HGCEEConfig=cms.PSet(
        adcNbits=cms.uint32(10),
        adcSaturation=cms.double(100),
        fCPerMIP=cms.vdouble(2.06, 3.43, 5.15, 3.43),
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
        fCPerMIP=cms.vdouble(2.06, 3.43, 5.15, 3.43),
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
process.hltHGCalUncalibRecHitDecompressed = process.hltHGCalUncalibRecHit.clone(
    HGCEEdigiCollection=cms.InputTag("hltHgcalDigisDecompressed", "EE"),
    HGCHEFdigiCollection=cms.InputTag("hltHgcalDigisDecompressed", "HEfront"),
    HGCHEBdigiCollection=cms.InputTag("hltHgcalDigisDecompressed", "HEback"),
)
process.hgcalUncalibRecHitRoundTripValidator = cms.EDAnalyzer(
    "HGCalUncalibRecHitRoundTripValidator",
    original=cms.InputTag("hltHGCalUncalibRecHit"),
    restored=cms.InputTag("hltHGCalUncalibRecHitDecompressed"),
)

process.convert = cms.Path(
    process.hltHgcalDigis
    + process.hltHGCalUncalibRecHit
    + process.hltHgcalDigisSoA
    + process.hltHgcalDigisDecompressed
    + process.hltHGCalUncalibRecHitDecompressed
    + process.hgcalUncalibRecHitRoundTripValidator
)

outputCommands = cms.untracked.vstring(
    "drop *",
    "keep *_hltHgcalDigis_*_ROUNDTRIP",
    "keep *_hltHgcalDigisSoA_*_ROUNDTRIP",
    "keep *_hltHgcalDigisDecompressed_*_ROUNDTRIP",
    "keep *_hltHGCalUncalibRecHit_*_ROUNDTRIP",
    "keep *_hltHGCalUncalibRecHitDecompressed_*_ROUNDTRIP",
)
process.output = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("file:hltHgcalDigisRecHitRoundTripLZMA4.root"),
    compressionAlgorithm=cms.untracked.string("LZMA"),
    compressionLevel=cms.untracked.int32(4),
    splitLevel=cms.untracked.int32(0),
    outputCommands=outputCommands,
)
process.outputZSTD = process.output.clone(
    fileName=cms.untracked.string("file:hltHgcalDigisRecHitRoundTripZSTD3.root"),
    compressionAlgorithm=cms.untracked.string("ZSTD"),
    compressionLevel=cms.untracked.int32(3),
)
process.end = cms.EndPath(process.output + process.outputZSTD)

process.options.wantSummary = True
process.options.numberOfStreams = 6
process.options.numberOfThreads = 12
