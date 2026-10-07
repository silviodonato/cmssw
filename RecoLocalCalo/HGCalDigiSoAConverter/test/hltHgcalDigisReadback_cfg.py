import FWCore.ParameterSet.Config as cms

process = cms.Process("READBACK")
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring("file:hltHgcalDigisRecHitRoundTripLZMA4.root"),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.hgcalUncalibRecHitRoundTripValidator = cms.EDAnalyzer(
    "HGCalUncalibRecHitRoundTripValidator",
    original=cms.InputTag("hltHGCalUncalibRecHit", "", "ROUNDTRIP"),
    restored=cms.InputTag("hltHGCalUncalibRecHitDecompressed", "", "ROUNDTRIP"),
)
process.readback = cms.Path(
    process.hgcalUncalibRecHitRoundTripValidator
)
