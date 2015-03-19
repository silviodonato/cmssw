'''

Creates L1ExtraNtuples (L1 Style) using a UCT->GT jump

Authors: L. Dodd, N. Woods, T. Perry, A. Levine,, S. Dasu, M. Cepeda, E. Friis (UW Madison)

'''

import FWCore.ParameterSet.Config as cms
import os

from FWCore.ParameterSet.VarParsing import VarParsing
process = cms.Process("ReRunningL1")


process.options = cms.untracked.PSet(
   wantSummary = cms.untracked.bool( True ),
#   multiProcesses=cms.untracked.PSet(
#        maxChildProcesses=cms.untracked.int32(20),
#        maxSequentialEventsPerChild=cms.untracked.uint32(1000))
)


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

#process.source = cms.Source ("PoolSource",
#    fileNames = cms.untracked.vstring(
#"file:/scratch/sdonato/TriggerRun2/CMSSW_7_1_0_pre7/src/Reco/MC.root"
#),
##                             fileNames = cms.untracked.vstring(
##"/store/mc/Fall13dr/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00005/6AF2C1E2-DF7F-E311-B452-003048679162.root",
##                             )   
#                             )


process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring())

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)


process.TFileService = cms.Service("TFileService", fileName = cms.string("ntuplaL1MinBiasFlatPU.root") ,      closeFileFast = cms.untracked.bool(True))
process.n = cms.EDAnalyzer("NtuplerL1Gen")
#process.path = cms.Path(process.n)



# Tested on Monte Carlo, for a test with data edit ahead
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS170_V5::All'

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
