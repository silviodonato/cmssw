import FWCore.ParameterSet.Config as cms

def customizeHLTforScoutingMuonNoVtx2025(process):
    # Create the new NoVtx2025 modules by cloning the NoVtx ones
    
    if hasattr(process, 'hltDoubleMuonL3PreFilteredScoutingDisplaced'):
        process.hltDoubleMuonL3PreFilteredScoutingNoVtx2025 = cms.EDFilter("HLTMuonL3PreFilter",
            MinN = cms.int32(2)
            # You can copy other needed config if required.
        )
        
    if hasattr(process, 'hltDisplacedmumuVtxNoMatchingProducerNoVtx'):
        process.hltDisplacedmumuVtxNoMatchingProducerNoVtx2025 = process.hltDisplacedmumuVtxNoMatchingProducerNoVtx.clone(
            Src = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltMuonEcalMFPFClusterIsoForMuonsNoVtx'):
        process.hltMuonEcalMFPFClusterIsoForMuonsNoVtx2025 = process.hltMuonEcalMFPFClusterIsoForMuonsNoVtx.clone(
            recoCandidateProducer = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltMuonHcalPFClusterIsoForMuonsNoVtx'):
        process.hltMuonHcalPFClusterIsoForMuonsNoVtx2025 = process.hltMuonHcalPFClusterIsoForMuonsNoVtx.clone(
            recoCandidateProducer = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltMuonTkRelIsolationCut0p09MapNoVtx'):
        process.hltMuonTkRelIsolationCut0p09MapNoVtx2025 = process.hltMuonTkRelIsolationCut0p09MapNoVtx.clone(
            inputMuonCollection = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx'):
        process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx2025 = process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx.clone()
        if hasattr(process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx2025, 'RegionPSet'):
            process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx2025.RegionPSet.input = cms.InputTag("hltIterL3MuonCandidatesNoVtx")

    if hasattr(process, 'hltRecHitInRegionForMuonsESNoVtx'):
        process.hltRecHitInRegionForMuonsESNoVtx2025 = process.hltRecHitInRegionForMuonsESNoVtx.clone(
            l1TagIsolated = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltRecHitInRegionForMuonsMFnoVtx'):
        process.hltRecHitInRegionForMuonsMFnoVtx2025 = process.hltRecHitInRegionForMuonsMFnoVtx.clone(
            l1TagIsolated = cms.InputTag("hltIterL3MuonCandidatesNoVtx")
        )

    if hasattr(process, 'hltScoutingMuonPackerNoVtx'):
        process.hltScoutingMuonPackerNoVtx2025 = process.hltScoutingMuonPackerNoVtx.clone(
            ChargedCandidates = cms.InputTag("hltIterL3MuonCandidatesNoVtx"),
            InputMuons = cms.InputTag("hltIterL3MuonsNoVtx"),
            InputLinks = cms.InputTag("hltL3MuonsIterL3LinksNoVtx"),
            Tracks = cms.InputTag("hltIterL3MuonAndMuonFromL1MergedNoVtx")
        )

    # Creating cloned sequences that replicate NoVtx but use the NoVtx2025 modules and apply requested additions.
    # Note: We must also clone the parent sequences all the way up since they now call newly named modules.

    if hasattr(process, 'HLTPFClusteringEcalMFForMuonsNoVtx'):
        process.HLTPFClusteringEcalMFForMuonsNoVtx2025 = process.HLTPFClusteringEcalMFForMuonsNoVtx.copy()
        process.HLTPFClusteringEcalMFForMuonsNoVtx2025.replace(process.hltRecHitInRegionForMuonsMFnoVtx, process.hltRecHitInRegionForMuonsMFnoVtx2025)
        process.HLTPFClusteringEcalMFForMuonsNoVtx2025.replace(process.hltRecHitInRegionForMuonsESNoVtx, process.hltRecHitInRegionForMuonsESNoVtx2025)

    if hasattr(process, 'HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx'):
        process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025 = process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx.copy()
        if hasattr(process, 'HLTPFClusteringEcalMFForMuonsNoVtx'):
            process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025.replace(process.HLTPFClusteringEcalMFForMuonsNoVtx, process.HLTPFClusteringEcalMFForMuonsNoVtx2025)
        process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025.replace(process.hltMuonEcalMFPFClusterIsoForMuonsNoVtx, process.hltMuonEcalMFPFClusterIsoForMuonsNoVtx2025)

    if hasattr(process, 'HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx'):
        process.HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025 = process.HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx.copy()
        process.HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025.replace(process.hltMuonHcalPFClusterIsoForMuonsNoVtx, process.hltMuonHcalPFClusterIsoForMuonsNoVtx2025)

    if hasattr(process, 'HLTIterativeTrackingL3MuonIteration0NoVtx'):
        process.HLTIterativeTrackingL3MuonIteration0NoVtx2025 = process.HLTIterativeTrackingL3MuonIteration0NoVtx.copy()
        process.HLTIterativeTrackingL3MuonIteration0NoVtx2025.replace(process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx, process.hltPixelTracksTrackingRegionsForSeedsL3MuonNoVtx2025)

    if hasattr(process, 'HLTTrackReconstructionForIsoL3MuonIter02NoVtx'):
        process.HLTTrackReconstructionForIsoL3MuonIter02NoVtx2025 = process.HLTTrackReconstructionForIsoL3MuonIter02NoVtx.copy()
        process.HLTTrackReconstructionForIsoL3MuonIter02NoVtx2025.replace(process.HLTIterativeTrackingL3MuonIteration0NoVtx, process.HLTIterativeTrackingL3MuonIteration0NoVtx2025)

    if hasattr(process, 'HLTMuIsolationSequenceNoVtx'):
        process.HLTMuIsolationSequenceNoVtx2025 = process.HLTMuIsolationSequenceNoVtx.copy()
        if hasattr(process, 'HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx'):
            process.HLTMuIsolationSequenceNoVtx2025.replace(process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx, process.HLTL3muonEcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025)
        if hasattr(process, 'HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx'):
            process.HLTMuIsolationSequenceNoVtx2025.replace(process.HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx, process.HLTL3muonHcalPFisorecoSequenceNoBoolsForMuonsNoVtx2025)
        if hasattr(process, 'HLTTrackReconstructionForIsoL3MuonIter02NoVtx'):
            process.HLTMuIsolationSequenceNoVtx2025.replace(process.HLTTrackReconstructionForIsoL3MuonIter02NoVtx, process.HLTTrackReconstructionForIsoL3MuonIter02NoVtx2025)
        process.HLTMuIsolationSequenceNoVtx2025.replace(process.hltMuonTkRelIsolationCut0p09MapNoVtx, process.hltMuonTkRelIsolationCut0p09MapNoVtx2025)

    if hasattr(process, 'HLTPFScoutingPackingSequence'):
        process.HLTPFScoutingPackingSequence2025 = process.HLTPFScoutingPackingSequence.copy()
        process.HLTPFScoutingPackingSequence2025.replace(process.hltScoutingMuonPackerNoVtx, process.hltScoutingMuonPackerNoVtx2025)

    if hasattr(process, 'HLTPFScoutingTrackingSequence'):
        process.HLTPFScoutingTrackingSequence2025 = process.HLTPFScoutingTrackingSequence.copy()
        process.HLTPFScoutingTrackingSequence2025.replace(process.hltDisplacedmumuVtxNoMatchingProducerNoVtx, process.hltDisplacedmumuVtxNoMatchingProducerNoVtx2025)
        process.HLTPFScoutingTrackingSequence2025.replace(process.HLTMuIsolationSequenceNoVtx, process.HLTMuIsolationSequenceNoVtx2025)
        process.HLTPFScoutingTrackingSequence2025.replace(process.HLTPFScoutingPackingSequence, process.HLTPFScoutingPackingSequence2025)
        
        # Apply requested additions and removals explicitly
        if hasattr(process, 'HLTL3DisplacedMuonRecoSequence') and hasattr(process, 'HLTL3muonrecoSequenceNoVtx'):
            process.HLTPFScoutingTrackingSequence2025.replace(process.HLTL3DisplacedMuonRecoSequence, process.HLTL3muonrecoSequenceNoVtx)

    if hasattr(process, 'HLTDoubleMuonScoutingNoVtx'):
        process.HLTDoubleMuonScoutingNoVtx2025 = process.HLTDoubleMuonScoutingNoVtx.copy()
        if hasattr(process, 'HLTL3DisplacedMuonRecoSequence') and hasattr(process, 'HLTL3muonrecoSequenceNoVtx'):
            process.HLTDoubleMuonScoutingNoVtx2025.replace(process.HLTL3DisplacedMuonRecoSequence, process.HLTL3muonrecoSequenceNoVtx)
        
        if hasattr(process, 'hltDoubleMuonL3PreFilteredScoutingDisplaced') and hasattr(process, 'hltDoubleMuonL3PreFilteredScoutingNoVtx2025'):
            process.HLTDoubleMuonScoutingNoVtx2025.replace(process.hltDoubleMuonL3PreFilteredScoutingDisplaced, process.hltDoubleMuonL3PreFilteredScoutingNoVtx2025)

    # Done, return modified process with appended elements
    return process

if __name__ == "__main__":
    import HLTrigger.Configuration.HLT_GRun_cff as HLT_GRun_cff
    import FWCore.ParameterSet.Config as cms
    
    print("=== Testing Customization Script (Add NoVtx2025 Strategy) ===")
    
    customizeHLTforScoutingMuonNoVtx2025(HLT_GRun_cff.fragment)
    
    assert hasattr(HLT_GRun_cff.fragment, 'HLTDoubleMuonScoutingNoVtx2025')
    assert hasattr(HLT_GRun_cff.fragment, 'hltScoutingMuonPackerNoVtx2025')
    
    print("\nOriginal HLTDoubleMuonScoutingNoVtx content (Should remain unchanged):")
    print(HLT_GRun_cff.fragment.HLTDoubleMuonScoutingNoVtx.dumpPython())
    
    print("\nNew HLTDoubleMuonScoutingNoVtx2025 content:")
    print(HLT_GRun_cff.fragment.HLTDoubleMuonScoutingNoVtx2025.dumpPython())
    
    print("\nNew HLTPFScoutingTrackingSequence2025 content:")
    print(HLT_GRun_cff.fragment.HLTPFScoutingTrackingSequence2025.dumpPython())
