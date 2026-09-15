import FWCore.ParameterSet.Config as cms
from stepHLT_HLT import process 

### stepHLT_HLT.py is generated from: (CMSSW_20_0_0_patch1)
#cmsDriver.py stepHLT  -s HLT:NGTScouting --process HLTX --conditions auto:phase2_realistic_T35 --datatier GEN-SIM-DIGI-RAW -n 10 --eventcontent FEVTDEBUGHLT --geometry ExtendedRun4D128 --era Phase2C26I13M9 --filein file:/shared/sdonato/HGCalClusters/CMSSW_20_0_0_patch1/src/CMSSW_20_0_0_patch1_RelValTTbar_14TeV_GEN-SIM-DIGI-RAW_PU_150X_mcRun4_realistic_v1_STD_D128_RegeneratedGS_PU_16Aug26-v2_2590000_85ca555d-58df-475c-ae33-bcd48789cf47.root --fileout file:step2_slim_postHLT.root --no_exec


process.hltHGCalUncalibRecHitCompressed = cms.EDProducer(
    "HGCalUncalibRecHitCompressor",
    src = cms.VInputTag(
        cms.InputTag("hltHGCalUncalibRecHit", "HGCEEUncalibRecHits"),
        cms.InputTag("hltHGCalUncalibRecHit", "HGCHEFUncalibRecHits"),
        cms.InputTag("hltHGCalUncalibRecHit", "HGCHEBUncalibRecHits"),
    ),
)

process.hltHGCalUncalibRecHitDecompressed = cms.EDProducer(
    "HGCalUncalibRecHitDecompressor",
    src = cms.VInputTag(
        cms.InputTag("hltHGCalUncalibRecHitCompressed", "HGCEEUncalibRecHits"),
        cms.InputTag("hltHGCalUncalibRecHitCompressed", "HGCHEFUncalibRecHits"),
        cms.InputTag("hltHGCalUncalibRecHitCompressed", "HGCHEBUncalibRecHits"),
    ),
)

#del process.simEcalUnsuppressedDigis
#del process.simHGCalUnsuppressedDigis
#del process.simHcalUnsuppressedDigis
#del process.simSiPixelDigis
#del process.simSiStripDigis

# HGCal local reco (produces HGCalUncalibRecHit from simHGCalUnsuppressedDigis)
#process.load('RecoLocalCalo.HGCalRecProducers.HGCalUncalibRecHit_cfi')

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('file:stepHLT_onlyHGCalCompression.root'), 
    outputCommands = cms.untracked.vstring(
        'drop *',

        #  # MTD/FTL
        # 'keep *_mix_FTLBarrel_*',
        # 'keep *_mix_FTLEndcap_*',

        # # HGCal
        # 'keep *_simHGCalUnsuppressedDigis_EE_*',
        # 'keep *_simHGCalUnsuppressedDigis_HEfront_*',
        # 'keep *_simHGCalUnsuppressedDigis_HEback_*',

        # # Tracker
        # 'keep *_simSiPixelDigis_Pixel_*',
        # 'keep *_mix_Tracker_*',

        # # ECAL
        # 'keep *_simEcalDigis_*_*',
        # 'keep FEDRawDataCollection_partialRawDataRepackerECAL_*_*',

        # # HCAL
        # 'keep *_simHcalDigis_*_*',

        # # Muon CSC
        # 'keep *_simMuonCSCDigis_*_*',

        # # Muon GEM
        # 'keep *_simMuonGEMDigis_*_*',
        # 'keep *_simMuonGEMPadDigis_*_*',
        # 'keep *_simMuonGEMPadDigiClusters_*_*',

        # # Muon RPC
        # 'keep *_simMuonRPCDigis_*_*',

        # # Muon DT
        # 'keep *_simMuonDTDigis_*_*',

        # #Additional
        # 'keep FEDRawDataCollection_rawDataCollector_*_*',
        # 'keep *_addPileupInfo_*_*',
        # 'keep *_mix_EBTimeDigi_*',
        # #'keep *_mix_MergedTrackTruth_*',
        # #'keep *_simSiPixelDigis_Tracker_*',
        # 'keep *_genParticles_*_*',
        # 'keep *_ak8GenJetsNoNu_*_*',
        # 'keep *_ak4GenJetsNoNu_*_*',
        # 'keep *_genMetTrue_*_*',
        # 'keep *_hltTriggerSummaryAOD_*_*',
        # 'keep *_TriggerResults_*_HLT',

        # Modify HGCal objects
        'keep HGCUncalibratedRecHitsSorted_hltHGCalUncalibRecHit_*_*',
        'keep *_hltHGCalUncalibRecHitCompressed_*_*',
        'keep HGCUncalibratedRecHitsSorted_hltHGCalUncalibRecHitDecompressed_*_*',

        # step3 RECOSIM additional branches
        #'keep *_prunedTrackingParticles_*_*',
        #'keep *_prunedDigiSimLinks_*_*',
        #'keep *_mix_MergedMtdTruth_*',
        #'keep *_mix_MergedMtdTruthLC_*',
        #'keep *_mix_MergedMtdTruthST_*',

        #RECO output
        #'keep *_mtdUncalibratedRecHits_FTLBarrel_*',
        #'keep *_mtdUncalibratedRecHits_FTLEndcap_*',
        #'keep *_mtdClusters_FTLBarrel_*',
        #'keep *_mtdClusters_FTLEndcap_*',
        #'keep *_mtdRecHits_FTLBarrel_*',
        #'keep *_mtdRecHits_FTLEndcap_*',
        #'keep *_mtdTrackingRecHits_*_*',
        #'keep *_trackExtenderWithMTD_*_*',
        #'keep *_ecalDigis_*_*',
        #'keep *_hcalDigis_*_*',
        #'keep *_muonCSCDigis_*_*',
        #'keep *_muonGEMDigis_*_*',
        #'keep *_muonRPCDigis_*_*',
        #'keep *_muonDTDigis_*_*',

        ### keep all HLT reco'd objects
        #'keep *_*_*_HLTX',
    ),
    #compressionAlgorithm = cms.untracked.string("LZMA"),
    #compressionLevel = cms.untracked.int32(4),
    compressionAlgorithm = cms.untracked.string("ZSTD"),
    compressionLevel = cms.untracked.int32(3),
    splitLevel = cms.untracked.int32(99)
)

## in CMSSW_20_0_X PFScouting is NGT scouting
process.DST_PFScouting = cms.Path(
    process.HLTBeginSequence
    + process.hltL1GTAcceptFilter
    + process.HLTRawToDigiSequence
    + process.HLTLocalrecoSequence
    + process.HLTTICLLocalRecoSequence
    + process.hltHGCalUncalibRecHitCompressed
    + process.hltHGCalUncalibRecHitDecompressed
)
## since CMSSW_20_1_X PFScouting is the "standard" scouting and NGT scouting has its own path:
#process.DST_NGTScouting = cms.Path(process.HLTBeginSequence+process.hltL1GTAcceptFilter+process.HLTRawToDigiSequence+process.HLTLocalrecoSequence+process.HLTTICLLocalRecoSequence)

#del process.endjob_step
del process.FEVTDEBUGHLToutput_step
#del process.DST_PFScouting

process.endjob_step = cms.EndPath(process.output)
print(process.schedule)

process.options.wantSummary = True
process.options.numberOfStreams = 4
process.options.numberOfThreads = 8

process.maxEvents.input = 8

#process.source.inputFiles = ["root://eoscms.cern.ch//store/relval/CMSSW_20_0_0_patch1/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_150X_mcRun4_realistic_v1_STD_D128_RegeneratedGS_PU_16Aug26-v2/2590000/c0bf8a3e-cf56-46af-b47e-527cd9313b14.root"]


process.output.overrideBranchesSplitLevel = cms.untracked.VPSet(
    cms.untracked.PSet(
        # The final "." is part of the actual EDM branch name.
        branch=cms.untracked.string(
            "HGCUncalibratedRecHitCompressedsSorted_"
            "hltHGCalUncalibRecHitCompressed_*_HLTX."
        ),
        splitLevel=cms.untracked.int32(99)
    ),
    cms.untracked.PSet(
        # The final "." is part of the actual EDM branch name.
        branch=cms.untracked.string(
            "HGCUncalibratedRecHitSorted_"
            "hltHGCalUncalibRecHit_*_HLTX."
        ),
        splitLevel=cms.untracked.int32(99)
    )
)