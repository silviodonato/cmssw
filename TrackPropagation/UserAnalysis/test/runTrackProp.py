import FWCore.ParameterSet.Config as cms

process = cms.Process("TrackAnalysis")

# Load necessary sequences for geometry and magnetic field
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('TrackingTools.TransientTrack.TransientTrackBuilder_cfi')
process.load('TrackingTools.MaterialEffects.MaterialPropagatorParabolicMf_cff')
process.load('RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilder_cfi')
process.load('RecoLocalTracker.SiPixelRecHits.PixelCPEGeneric_cfi')
process.load('RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run3_data_prompt', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/scratch/nandan/step5_RAW2DIGI_L1Reco_RECO.root'
    )
)

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO')
    )
)

process.load('RecoLocalTracker.SiStripRecHitConverter.StripCPEESProducer_cfi')

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("output.root")
)

process.trackPropAnalyzer = cms.EDAnalyzer('TrackPropAnalyzer',
    tracks = cms.InputTag("generalTracks", "", "reRECO")
)

process.p = cms.Path(process.trackPropAnalyzer)
