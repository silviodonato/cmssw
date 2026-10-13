import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing("analysis")
options.setDefault("inputFiles", ["file:hltHgcalDigisRecHitRoundTripZSTD3.root"])
options.setDefault("maxEvents", -1)
options.setDefault("outputFile", "hgcalDigiCorrelations.root")
options.parseArguments()

process = cms.Process("DIGINTUPLE")

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.inputFiles),
    dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
    inputCommands=cms.untracked.vstring(
        "drop *", "keep *_hltHgcalDigisDecompressed_EE_ROUNDTRIP"
    ),
)

process.TFileService = cms.Service(
    "TFileService", fileName=cms.string(options.outputFile)
)
process.ntuplizer = cms.EDAnalyzer(
    "HGCalDigiNtuplizer",
    digis=cms.InputTag("hltHgcalDigisDecompressed", "EE", "ROUNDTRIP"),
)
process.path = cms.Path(process.ntuplizer)
