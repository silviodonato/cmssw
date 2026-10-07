import FWCore.ParameterSet.Config as cms

process = cms.Process("ROUNDTRIP")
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
    "HGCalDigisToLosslessSoA",
    src=cms.InputTag("hltHgcalDigis"),
)
process.hltHgcalDigisDecompressed = cms.EDProducer(
    "HGCalDigisFromLosslessSoA",
    src=cms.InputTag("hltHgcalDigisSoA"),
)
process.hgcalDigiRoundTripValidator = cms.EDAnalyzer(
    "HGCalDigiRoundTripValidator",
    original=cms.InputTag("hltHgcalDigis"),
    restored=cms.InputTag("hltHgcalDigisDecompressed"),
)
process.convert = cms.Path(
    process.hltHgcalDigis
    + process.hltHgcalDigisSoA
    + process.hltHgcalDigisDecompressed
    + process.hgcalDigiRoundTripValidator
)

process.output = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("file:hltHgcalDigisRoundTripLZMA4.root"),
    compressionAlgorithm=cms.untracked.string("LZMA"),
    compressionLevel=cms.untracked.int32(4),
    splitLevel=cms.untracked.int32(0),
    outputCommands=cms.untracked.vstring(
        "drop *",
        "keep *_hltHgcalDigis_*_ROUNDTRIP",
        "keep *_hltHgcalDigisSoA_*_ROUNDTRIP",
        "keep *_hltHgcalDigisDecompressed_*_ROUNDTRIP",
    ),
)

process.outputZSTD = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("file:hltHgcalDigisRoundTripZSTD3.root"),
    compressionAlgorithm=cms.untracked.string("ZSTD"),
    compressionLevel=cms.untracked.int32(3),
    splitLevel=cms.untracked.int32(0),
    outputCommands=cms.untracked.vstring(
        "drop *",
        "keep *_hltHgcalDigis_*_ROUNDTRIP",
        "keep *_hltHgcalDigisSoA_*_ROUNDTRIP",
        "keep *_hltHgcalDigisDecompressed_*_ROUNDTRIP",
    ),
)

process.end = cms.EndPath(process.output + process.outputZSTD)

process.options.wantSummary = True
process.options.numberOfStreams = 4
process.options.numberOfThreads = 8