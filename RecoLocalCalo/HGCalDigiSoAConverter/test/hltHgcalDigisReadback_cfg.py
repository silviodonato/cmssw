import FWCore.ParameterSet.Config as cms

process = cms.Process("READBACK")
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring("file:hltHgcalDigisRoundTripLZMA4.root"),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.hgcalDigiRoundTripValidator = cms.EDAnalyzer(
    "HGCalDigiRoundTripValidator",
    original=cms.InputTag("hltHgcalDigis"),
    restored=cms.InputTag("hltHgcalDigisDecompressed"),
)
process.readback = cms.Path(
    process.hgcalDigiRoundTripValidator
)
