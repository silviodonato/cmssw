'''

Creates L1ExtraNtuples (L1 Style) using a UCT->GT jump

Authors: L. Dodd, N. Woods, T. Perry, A. Levine,, S. Dasu, M. Cepeda, E. Friis (UW Madison)

'''

import FWCore.ParameterSet.Config as cms
import os

from FWCore.ParameterSet.VarParsing import VarParsing
process = cms.Process("ReRunningL1")

# Get command line options
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')

options.register(
    'isMC',
    1,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    'Set to 1 for simulated samples - updates GT, emulates HCAL TPGs.')

options.parseArguments()


process.source = cms.Source ("PoolSource",
    fileNames = cms.untracked.vstring(
#"file:/scratch/sdonato/TriggerRun2/CMSSW_7_1_0_pre7/src/Reco/MC.root"
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/004EE9D6-EF6C-E311-968E-848F69FD29D6.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/029D7913-E16C-E311-8839-00266CFAE30C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/069906ED-F26C-E311-AE31-008CFA001F1C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/06E02049-E56C-E311-A979-00266CF9BBE4.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/087040FF-F06C-E311-83F7-00266CFAE838.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/0A639047-E66C-E311-954B-848F69FD291F.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/0E46AE45-CF6C-E311-8598-848F69FD4E41.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1A707E6D-EC6C-E311-8C44-00A0D1EEA838.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1C7E3782-D26C-E311-9EEA-7845C4FC3A58.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1E35E13D-EB6C-E311-9E41-7845C4FC3B69.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/262963B4-E76C-E311-83BA-7845C4FC35F0.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/2A878197-E86C-E311-8E52-008CFA008D0C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/2CA5E6D5-D96C-E311-A26D-7845C4FC3614.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/2E6929FC-EB6C-E311-9E01-00266CF9B7AC.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/32964FF8-DE6C-E311-8F6B-00A0D1EE8D7C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/4EE19C8A-E36C-E311-AB07-7845C4FC3AC1.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/508FF63E-DF6C-E311-B068-00266CF9B414.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/52984A76-E46C-E311-A8B7-848F69FD29AF.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/5667F126-EC6C-E311-BCBD-00266CFAE69C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/5850D4F7-DE6C-E311-92CC-001D09FDD7D1.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/5CEA5343-EE6C-E311-B095-7845C4FC3AFD.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/68F53F3A-E06C-E311-A1E4-848F69FD28EC.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/7426913F-E36C-E311-8ECB-00266CF9C22C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/74845BB8-E76C-E311-ABD8-7845C4FC39DA.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/76B93AC2-E06C-E311-8775-00266CF9B414.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/76D3A2B9-E96C-E311-9A09-008CFA002830.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/780BA3C7-F16C-E311-8C81-00A0D1EE95CC.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/7A726DE0-E16C-E311-9D12-00266CF275E0.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8269E6D1-E66C-E311-AA92-00266CF27130.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/86D69A70-CD6C-E311-9BE7-008CFA000BB8.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/88D08BDE-CC6C-E311-AA7B-008CFA000BB8.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8A258707-DE6C-E311-9C89-848F69FD4592.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8C6D4AE1-D36C-E311-835E-00A0D1EEF328.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8C7A3165-E36C-E311-BA02-848F69FD2520.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9019DAFE-E46C-E311-8646-00A0D1EE26D0.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/94E28ED8-E86C-E311-BAEE-008CFA000280.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9C8216A2-EA6C-E311-8009-008CFA001DB8.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A0B08082-DC6C-E311-B64E-7845C4FC37A9.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A2FA67FD-E76C-E311-B61F-00266CF271E8.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A8230230-EC6C-E311-B0AB-848F69FD47A5.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/AE122DFE-D56C-E311-BA58-7845C4FC34BB.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B61EF055-ED6C-E311-A694-7845C4FC3A7F.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B8147028-E46C-E311-91D2-7845C4FC39E3.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/BAB01697-DB6C-E311-ACF4-7845C4FC3A2B.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/BEC49C4B-E36C-E311-B7CE-7845C4FC378E.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C622D15A-CB6C-E311-AF8D-00A0D1EEF5B4.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C8768242-EB6C-E311-B033-848F69FD29DF.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C8B7388C-D76C-E311-A25B-00266CF20044.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D28C009D-E86C-E311-827A-848F69FD0884.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D4E3B51C-DD6C-E311-936F-7845C4FC3A2B.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/DE8AE3A6-D36C-E311-8EF5-848F69FD0884.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E0CB5938-CA6C-E311-AFAD-00266CF9B184.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E2B45C6C-E96C-E311-BE71-00A0D1EE271C.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E40FE8BA-E96C-E311-8278-7845C4FC3983.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EC0570BB-F46C-E311-8730-008CFA002E80.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EEBDBA7F-FE6C-E311-8E8E-00A0D1EEF364.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F0E1C2B0-EA6C-E311-B907-7845C4FC3AB8.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F6BF4EE8-ED6C-E311-89B7-008CFA007B98.root',
       '/store/mc/Fall13dr/VBF_HToBB_M-125_13TeV-powheg-pythia6/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F88D0BEB-E56C-E311-801C-848F69FD24D2.root'
),
#                             fileNames = cms.untracked.vstring(
#"/store/mc/Fall13dr/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00005/6AF2C1E2-DF7F-E311-B452-003048679162.root",
#                             )   
                             )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.TFileService = cms.Service("TFileService", fileName = cms.string("ntupla2L1VBF.root") ,      closeFileFast = cms.untracked.bool(True))
process.n = cms.EDAnalyzer("NtuplerL1Gen")
#process.path = cms.Path(process.n)



# Tested on Monte Carlo, for a test with data edit ahead
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'

# Load emulation and RECO sequences
if not options.isMC:
    process.load("L1Trigger.UCT2015.emulation_cfi")
    print "Running on data!"     
else:
    process.load("L1Trigger.UCT2015.emulationMC_cfi")

# Load sequences
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("L1Trigger.UCT2015.uctl1extraparticles_cfi")

process.p1 = cms.Path(
    process.emulationSequence *
    process.uct2015L1Extra *
    process.n
       #  *process.YourFavoritePlottingRoutine  --> This ends at l1extra production, anything after is up to the analyst 
)

# Make the framework shut up.
#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')

## Output definition
#process.output = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string('out.root'),
#    outputCommands = cms.untracked.vstring('drop *',
#          'keep *_*_*_ReRunningL1',
#          'keep *_l1extraParticles*_*_*') 
#)


# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:upgradePLS1')
    process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
    process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
    for pset in process.GlobalTag.toGet.value():
        pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#   Fix for multi-run processing:
    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
