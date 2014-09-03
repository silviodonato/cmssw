from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'NeutrinoL1FlatPUFromAODSIM_CRAB3'
config.General.workArea = 'crab_projects'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'ntuplerL1MinBiasAODSIM.py'

config.section_("Data")
config.Data.inputDataset = '/Neutrino_Pt-2to20_gun/Spring14dr-Flat20to50_POSTLS170_V5-v1/AODSIM'
config.Data.dbsUrl = 'global'
config.Data.splitting = 'FileBased'
config.Data.publication = True
config.Data.unitsPerJob = 30
config.Data.totalUnits = -1
#config.Data.publishDbsUrl = 'test'
config.Data.publishDataName = 'NeutrinoL1FlatPUFromAODSIM_CRAB3'

config.section_("Site")
config.Site.storageSite = 'T2_IT_Pisa'
