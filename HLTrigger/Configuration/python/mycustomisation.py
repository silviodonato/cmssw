import FWCore.ParameterSet.Config as cms

def replaceTrackMerger(process):
    if not 'hltScoutingTracks' in process.__dict__:
        return process
    
    # replace TrackListMerger with TrackSimpleMerger
    process.hltScoutingTracks = cms.EDProducer('TrackSimpleMerger',
       src = process.hltScoutingTracks.TrackProducers
    )
    
    return process

