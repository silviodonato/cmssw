import FWCore.ParameterSet.Config as cms

process = cms.Process("SIMPLE")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/scratch/nandan/step5_RAW2DIGI_L1Reco_RECO.root'
    )
)

# Standard geometry and magnetic field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '140X_dataRun3_v3', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_dataRun3_v3', '')

# Need to load the TransientTrackBuilder and Propagators
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('TrackingTools.MaterialEffects.MaterialPropagatorParabolicMf_cff')

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("track_prop_simple.root")
)

process.trackPropSimple = cms.EDAnalyzer('TrackPropSimple',
    tracks = cms.InputTag("generalTracks", "", "reRECO")
)

process.p = cms.Path(process.trackPropSimple)
