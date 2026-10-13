import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.register('module', 'hltHgcalDigisDecompressed', options.multiplicity.singleton, options.varType.string, 'Digi module')
options.register('instance', 'EE', options.multiplicity.singleton, options.varType.string, 'EE or HEfront')
options.register('processName', 'ROUNDTRIP', options.multiplicity.singleton, options.varType.string, 'Digi process')
options.register('jsonOutput', 'hgcal-d128-cells.json', options.multiplicity.singleton, options.varType.string, 'Output JSON')
options.parseArguments()
if options.instance not in ('EE', 'HEfront'):
    raise ValueError('Only silicon EE and HEfront collections are supported')
process = cms.Process('DETIDVIEW')
process.load('Configuration.Geometry.GeometryExtendedRun4D128Reco_cff')
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1))
process.source = cms.Source('PoolSource', fileNames=cms.untracked.vstring(options.inputFiles),
    dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
    inputCommands=cms.untracked.vstring('drop *',
        f'keep *_{options.module}_{options.instance}_{options.processName}'))
process.export = cms.EDAnalyzer('HGCalDigiGeometryExport',
    src=cms.InputTag(options.module, options.instance, options.processName),
    geometryLabel=cms.string('HGCalEESensitive' if options.instance == 'EE' else 'HGCalHESiliconSensitive'),
    output=cms.string(options.jsonOutput))
process.path = cms.Path(process.export)
