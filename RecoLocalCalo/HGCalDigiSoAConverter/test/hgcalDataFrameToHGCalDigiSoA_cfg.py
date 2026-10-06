import FWCore.ParameterSet.Config as cms

process = cms.Process("CONVERT")
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
#        "file:/shared/sdonato/CompressedHGCalRecHits/CMSSW_20_0_0_patch1/src/stepHLT_all_onlyHGCalCompression.root",
        "file:/cms-hlt-nfs/user/sdonato/CMSSW_20_0_0_patch1_RelValTTbar_14TeV_GEN-SIM-DIGI-RAW_PU_150X_mcRun4_realistic_v1_STD_D128_RegeneratedGS_PU_16Aug26-v2_2590000_85ca555d-58df-475c-ae33-bcd48789cf47.root"
    ),
    inputCommands=cms.untracked.vstring(
        "keep *",
        "drop HGCUncalibratedRecHitCompressedsSorted_*_*_*",
    ),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1))


process.hltHgcalDigis = cms.EDProducer("HGCalRawToDigiFake",
    bhDigis = cms.InputTag("simHGCalUnsuppressedDigis","HEback"),
    eeDigis = cms.InputTag("simHGCalUnsuppressedDigis","EE"),
    fhDigis = cms.InputTag("simHGCalUnsuppressedDigis","HEfront"),
    mightGet = cms.optional.untracked.vstring
)  

process.hgcalDataFrameToHGCalDigiSoA = cms.EDProducer(
    "HGCalDataFrameToHGCalDigiSoA",
    src=cms.InputTag("simHGCalUnsuppressedDigis", "EE", "HLT"),
    sampleIndex=cms.uint32(2),
)
process.convert = cms.Path(process.hgcalDataFrameToHGCalDigiSoA + process.hltHgcalDigis)

process.output = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("file:hgcalDataFrameToHGCalDigiSoA.root"),
    outputCommands=cms.untracked.vstring(
        "drop *",
        "keep *_hgcalDataFrameToHGCalDigiSoA_*_*",
        "keep *_*hltHgcalDigis*_*_*",
    ),
)
process.end = cms.EndPath(process.output)
