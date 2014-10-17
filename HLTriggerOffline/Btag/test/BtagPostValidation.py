import FWCore.ParameterSet.Config as cms
#denominator trigger
process.triggerSelection = cms.EDFilter( "TriggerResultsFilter",
    triggerConditions = cms.vstring(
      "HLT_PFMET170*"),
    hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
#    l1tResults = cms.InputTag( "gtDigis" ),
#    l1tIgnoreMask = cms.bool( False ),
#    l1techIgnorePrescales = cms.bool( False ),
#    daqPartitions = cms.uint32( 1 ),
    throw = cms.bool( True )
)
#define bTagPostValidation for the b-tag DQM validation (efficiency and mistagrate plot)
process.bTagPostValidation = cms.EDAnalyzer("HLTBTagHarvestingAnalyzer",
HLTPathNames = (
#    'HLT_BTagCSV07_v1', 
#    'HLT_PFMHT100_SingleCentralJet60_BTagCSV0p6_v1', 
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1'
    ),
histoName	= cms.vstring(
#	'hltL3CombinedSecondaryVertexBJetTags',
#	'hltL3CombinedSecondaryVertexBJetTags',
	'hltL3CombinedSecondaryVertexBJetTags',
	),
minTag	= cms.double(0.6),
# MC stuff
mcFlavours = cms.PSet(
light = cms.vuint32(1, 2, 3, 21), # udsg
c = cms.vuint32(4),
b = cms.vuint32(5),
g = cms.vuint32(21),
uds = cms.vuint32(1, 2, 3)
)
)
#put all in a path
process.DQM_BTag = cms.Sequence(
process.triggerSelection
+	process.bTagPostValidation
)	

