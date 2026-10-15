import FWCore.ParameterSet.Config as cms

process = cms.Process("CONVERT")
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "file:/shared/sdonato/CompressedHGCalRecHits/CMSSW_20_0_0_patch1/src/stepHLT_all_onlyHGCalCompression.root"
    ),
    inputCommands=cms.untracked.vstring(
        "keep *",
        "drop HGCUncalibratedRecHitCompressedsSorted_*_*_*",
    ),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(2))

process.qie10ToHGCalDigiSoA = cms.EDProducer(
    "QIE10ToHGCalDigiSoA",
    src=cms.InputTag("hltHcalDigis", "", "HLTX"),
)
process.convert = cms.Path(process.qie10ToHGCalDigiSoA)

process.output = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("file:qie10ToHGCalDigiSoA.root"),
    outputCommands=cms.untracked.vstring(
        "drop *",
        "keep *_qie10ToHGCalDigiSoA_*_*",
    ),
)
process.end = cms.EndPath(process.output)
