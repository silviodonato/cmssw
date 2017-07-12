import FWCore.ParameterSet.Config as cms

process = cms.Process("FastPVntupler")


process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")


from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag
process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = '92X_upgrade2017_realistic_v2')

#process.trackerTopology = cms.ESProducer("TrackerTopologyEP",
#    appendToDataLabel = cms.string('')
#)

#process.TrackerDigiGeometryESModule = cms.ESProducer("TrackerDigiGeometryESModule",
#    alignmentsLabel = cms.string(''),
#    appendToDataLabel = cms.string(''),
#    applyAlignment = cms.bool(True),
#    fromDDD = cms.bool(False)
#)



process.hltESPPixelCPEGeneric = cms.ESProducer("PixelCPEGenericESProducer",
    Alpha2Order = cms.bool(True),
    ClusterProbComputationFlag = cms.int32(0),
    ComponentName = cms.string('hltESPPixelCPEGeneric'),
    DoCosmics = cms.bool(False),
    EdgeClusterErrorX = cms.double(50.0),
    EdgeClusterErrorY = cms.double(85.0),
    IrradiationBiasCorrection = cms.bool(False),
    LoadTemplatesFromDB = cms.bool(True),
    MagneticFieldRecord = cms.ESInputTag(""),
    PixelErrorParametrization = cms.string('NOTcmsim'),
    TruncatePixelCharge = cms.bool(True),
    UseErrorsFromTemplates = cms.bool(True),
    eff_charge_cut_highX = cms.double(1.0),
    eff_charge_cut_highY = cms.double(1.0),
    eff_charge_cut_lowX = cms.double(0.0),
    eff_charge_cut_lowY = cms.double(0.0),
    inflate_all_errors_no_trk_angle = cms.bool(False),
    inflate_errors = cms.bool(False),
    size_cutX = cms.double(3.0),
    size_cutY = cms.double(3.0),
    useLAAlignmentOffsets = cms.bool(False),
    useLAWidthFromDB = cms.bool(False)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    #skipEvents =  cms.untracked.uint32(58),
    fileNames = 
cms.untracked.vstring(
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/38C1EB3C-2F5C-E711-9ED0-0CC47A7C34E6.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/4AA7BDA8-305C-E711-BA9D-0025905B859E.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/70EED4F9-325C-E711-9052-0025905A60B0.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/76FE0237-2F5C-E711-932E-0025905A60F8.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/804A388C-305C-E711-A8A7-0025905A48BA.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/903960F9-325C-E711-9DA9-0CC47A7C35D2.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/90A7C893-305C-E711-BECB-0025905A6104.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/90FBB504-335C-E711-8010-0025905A48D8.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/9C571D39-2F5C-E711-A102-0025905A60D0.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/A0BA084B-2F5C-E711-A3E2-0CC47A4D7668.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/B09EA5E1-2F5C-E711-9809-0025905A610C.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/BAD6A48C-2F5C-E711-90BE-0025905B8592.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/CA3FDE88-305C-E711-85B2-0CC47A4D7678.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/D87AC36D-315C-E711-B698-0025905A60B0.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/E609A59A-305C-E711-AD46-0025905A48F2.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/F24DB036-2F5C-E711-BE35-0CC47A4D76D2.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/F4C72212-335C-E711-8AF5-0025905A6138.root",
#    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_92X_upgrade2017_realistic_v2-v1/00000/F6B89235-2F5C-E711-846D-0025905A60F8.root",

    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/045CCCB4-3D5C-E711-87E7-0CC47A4D7600.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/0A1873F6-3D5C-E711-B2D3-0025905A6122.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/2EF87121-3D5C-E711-802E-0CC47A78A3B4.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/56B45AB3-3D5C-E711-8C92-0CC47A4D7644.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/622E725D-3C5C-E711-87B5-0CC47A7C3432.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/9609D422-3D5C-E711-92E4-0025905A6136.root",
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_9_2_4/RelValTTbar_13/GEN-SIM-DIGI-RAW/92X_upgrade2017_realistic_v2-v1/00000/DA3898F4-3D5C-E711-A9B2-0025905A611E.root",
    
),
)

process.fastPV = cms.EDAnalyzer("FastPVNtuplizer")
process.p = cms.Path(process.fastPV)

process.TFileService = cms.Service("TFileService", fileName = cms.string("fastPV.root") )
