from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'ZnnHbbL1FlatPUFromAODSIM_CRAB3'
config.General.workArea = 'crab_projects'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'ntuplerL1ZnnHbbAODSIM.py'

config.section_("Data")
config.Data.inputDataset = '/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/Spring14dr-Flat20to50_POSTLS170_V5-v1/GEN-SIM-RAW'
config.Data.dbsUrl = 'global'
config.Data.splitting = 'FileBased'
config.Data.publication = True
config.Data.unitsPerJob = 12
config.Data.totalUnits = -1
#config.Data.publishDbsUrl = 'test'
config.Data.publishDataName = 'ZnnHbbL1FlatPUFromAODSIM_CRAB3'

config.section_("Site")
config.Site.storageSite = 'T2_IT_Pisa'
