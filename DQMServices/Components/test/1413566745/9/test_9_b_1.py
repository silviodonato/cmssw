# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: test_9_b_1 -s HARVESTING:validationHarvestingFS --no_exec --harvesting AtRunEnd --conditions auto:startup --filetype DQM --filein file:test_9_a_1_inDQM.root --python_filename=test_9_b_1.py --mc --customise DQMServices/Components/test/customHarvesting.py
import FWCore.ParameterSet.Config as cms

process = cms.Process('HARVESTING')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EDMtoMEAtRunEnd_cff')
process.load('Configuration.StandardSequences.Harvesting_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

# Input source
process.source = cms.Source("DQMRootSource",
    fileNames = cms.untracked.vstring('file:test_9_a_1_inDQM.root')
)

process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound'),
    fileMode = cms.untracked.string('FULLMERGE')
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('test_9_b_1 nevts:1'),
    name = cms.untracked.string('Applications')
)

# Output definition

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:startup', '')

# Path and EndPath definitions
process.edmtome_step = cms.Path(process.EDMtoME)
process.validationprodHarvesting = cms.Path(process.hltpostvalidation_prod+process.postValidation_gen)
process.validationHarvesting = cms.Path(process.postValidation+process.hltpostvalidation+process.postValidation_gen)
process.dqmHarvestingPOGMC = cms.Path(process.DQMOffline_SecondStep_PrePOGMC)
process.validationpreprodHarvesting = cms.Path(process.postValidation_preprod+process.hltpostvalidation_preprod+process.postValidation_gen)
process.validationHarvestingHI = cms.Path(process.postValidationHI)
process.genHarvesting = cms.Path(process.postValidation_gen)
process.dqmHarvestingPOG = cms.Path(process.DQMOffline_SecondStep_PrePOG)
process.alcaHarvesting = cms.Path()
process.dqmHarvesting = cms.Path(process.DQMOffline_SecondStep+process.DQMOffline_Certification)
process.dqmsave_step = cms.Path(process.DQMSaver)

# Schedule definition
process.schedule = cms.Schedule(process.edmtome_step,process.validationHarvestingFS,process.dqmsave_step)

# customisation of the process.

# Automatic addition of the customisation function from DQMServices.Components.test.customHarvesting
from DQMServices.Components.test.customHarvesting import customise 

#call to customisation function customise imported from DQMServices.Components.test.customHarvesting
process = customise(process)

# End of customisation functions

