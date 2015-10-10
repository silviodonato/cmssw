datasets=[
'/HLTPhysics3/Run2015D-PromptReco-v3/AOD',
]

name = 'triggerNtupleAOD_FWLite_test_v24'

from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = name+"_"+datasets[0].split('/')[1]
config.General.workArea = 'crab_'+name
config.General.transferLogs=True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'crab_fake_pset.py'
config.JobType.scriptExe = 'crab_script.sh'
config.JobType.numCores = 1
import os
os.system("tar czf python.tar.gz --dereference --directory $CMSSW_BASE python")
os.system("voms-proxy-info -path | xargs -i  cp {}  .")
config.JobType.inputFiles = [
                             'fwlite_config.py',
                             'script.py',
                             'python.tar.gz',
                             'x509up_u9300',
]
#config.JobType.outputFiles = ['tree.root']

config.section_("Data")
config.Data.inputDataset = datasets[0]
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1 ##just for test##
config.Data.totalUnits = 1 ##just for test##
config.Data.outLFNDirBase = '/store/user/sdonato/' + name
config.Data.publication = True
config.Data.publishDataName = name+"_"+datasets[0].split('/')[1]
#config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'

config.section_("Site")
config.Site.storageSite = "T2_IT_Pisa"
