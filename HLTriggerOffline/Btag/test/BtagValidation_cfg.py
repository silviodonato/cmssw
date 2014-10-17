import FWCore.ParameterSet.Config as cms
process = cms.Process("HLTBTAG")
from PhysicsTools.PatAlgos.tools.coreTools import *	
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load("DQMServices.Components.EDMtoMEConverter_cff")
process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
#load hltJetMCTools sequence for the jet/partons matching
process.load("HLTriggerOffline.Btag.hltJetMCTools_cff")
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
#correct the jet used for the matching
process.hltJetsbyRef.jets = cms.InputTag("hltSelector4CentralJetsL1FastJet")
#define VertexValidationVertices for the vertex DQM validation
process.VertexValidationVertices= cms.EDAnalyzer("HLTVertexPerformanceAnalyzer",
TriggerResults = cms.InputTag('TriggerResults','',"HLT"),
HLTPathNames =cms.vstring(
#    'HLT_BTagCSV07_v1', 
#    'HLT_BTagCSV07_v1', 
#    'HLT_BTagCSV07_v1', 
#    'HLT_PFMHT100_SingleCentralJet60_BTagCSV0p6_v1', 
#    'HLT_PFMHT100_SingleCentralJet60_BTagCSV0p6_v1', 
#    'HLT_PFMHT100_SingleCentralJet60_BTagCSV0p6_v1', 
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1', 
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1', 
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1'
    ),
Vertex = cms.VInputTag(
#	cms.InputTag("hltFastPrimaryVertex"), 
#	cms.InputTag("hltFastPVPixelVertices"), 
	cms.InputTag("hltVerticesL3"), 
	cms.InputTag("hltFastPrimaryVertex"), 
	cms.InputTag("hltFastPVPixelVertices"),
)
)

#define bTagValidation for the b-tag DQM validation (distribution plot)
process.bTagValidation = cms.EDAnalyzer("HLTBTagPerformanceAnalyzer",
TriggerResults = cms.InputTag('TriggerResults','','HLT'),
HLTPathNames = cms.vstring(
#    'HLT_BTagCSV07_v1', 
#    'HLT_PFMHT100_SingleCentralJet60_BTagCSV0p6_v1', 
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1'
    ),
JetTag = cms.VInputTag(
#	cms.InputTag("hltL3CombinedSecondaryVertexBJetTags"), 
	cms.InputTag("hltL3CombinedSecondaryVertexBJetTags"), 
#	cms.InputTag("hltL3CombinedSecondaryVertexBJetTags")
	),
MinJetPT = cms.double(20),
mcFlavours = cms.PSet(
light = cms.vuint32(1, 2, 3, 21), # udsg
c = cms.vuint32(4),
b = cms.vuint32(5),
g = cms.vuint32(21),
uds = cms.vuint32(1, 2, 3)
),
mcPartons = cms.InputTag("hltJetsbyValAlgo")
)

#put all in a path
process.DQM_BTag_Validation = cms.Sequence(
process.triggerSelection
+	process.hltJetMCTools
+	process.VertexValidationVertices
+	process.bTagValidation
)	

