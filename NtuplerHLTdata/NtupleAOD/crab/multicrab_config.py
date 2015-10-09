datasets=[
#    '/MuonEG/Run2015B-PromptReco-v1/AOD',
#    '/SingleMu/Run2015B-PromptReco-v1/AOD',
#    '/SingleMuon/Run2015B-PromptReco-v1/AOD',
#    '/SinglePhoton/Run2015B-PromptReco-v1/AOD',
#    '/Tau/Run2015B-PromptReco-v1/AOD',
#    '/MuOnia/Run2015B-PromptReco-v1/AOD',
#    '/MET/Run2015B-PromptReco-v1/AOD',
#    '/JetHT/Run2015B-PromptReco-v1/AOD',
#    '/Jet/Run2015B-PromptReco-v1/AOD',
#    '/HTMHT/Run2015B-PromptReco-v1/AOD',
#    '/BTagCSV/Run2015B-PromptReco-v1/AOD',
#    '/BTagMu/Run2015B-PromptReco-v1/AOD',
#    '/DisplacedJet/Run2015B-PromptReco-v1/AOD',
#    '/DoubleEG/Run2015B-PromptReco-v1/AOD',
#    '/DoubleMuon/Run2015B-PromptReco-v1/AOD',
#    '/DoubleMuonLowMass/Run2015B-PromptReco-v1/AOD',
#    '/SingleElectron/Run2015B-PromptReco-v1/AOD',

#/ZH_HToBB_ZToLL_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM
#/ZH_HToBB_ZToNuNu_M130_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM
#/WplusH_HToBB_WToLNu_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM
#/VBFHToBB_M-130_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM
#/TT_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM

    '/ZH_HToBB_ZToLL_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/ZH_HToBB_ZToNuNu_M130_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/WplusH_HToBB_WToLNu_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/VBFHToBB_M-130_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/AODSIM',
    '/TT_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/AODSIM',

#    '/ZH_HToBB_ZToLL_M-125_13TeV_powheg-herwigpp/Phys14DR-PU20bx25_tsg_PHYS14_25_V1-v1/AODSIM',
#    '/WH_HToBB_WToLNu_M-125_13TeV_powheg-herwigpp/Phys14DR-PU20bx25_tsg_PHYS14_25_V1-v1/AODSIM',
#    '/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/Phys14DR-PU20bx25_tsg_PHYS14_25_V1-v1/AODSIM',
#    '/VBF_HToBB_M-125_13TeV-powheg-pythia6/Phys14DR-PU20bx25_tsg_PHYS14_25_V1-v1/AODSIM',
#    '/TT_Tune4C_13TeV-pythia8-tauola/Phys14DR-PU20bx25_tsg_PHYS14_25_V1-v1/AODSIM',

]
if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand
    
    def submit(config):
        res = crabCommand('submit', config = config)
    
    from CRABClient.UserUtilities import config
    config = config()
    for dataset in datasets:
        name = 'triggerNtupleAOD_FWLite_v20'
        config.section_("General")
        config.General.workArea = 'crab_'+name
        config.General.transferLogs=True
        config.General.requestName = name+"_"+dataset.split('/')[1]

        config.section_("JobType")
        config.JobType.pluginName = 'Analysis'
        config.JobType.psetName = 'crab_fake_pset.py'
        config.JobType.scriptExe = 'crab_script.sh'
        import os
        os.system("tar czf python.tar.gz --dereference --directory $CMSSW_BASE python")
        config.JobType.inputFiles = [
                                     'fwlite_config.py',
                                     'script.py',
                                     'python.tar.gz',
        ]
        
        config.section_("Data")
        config.Data.inputDBS = 'global'
        config.Data.splitting = 'FileBased'
        config.Data.unitsPerJob = 20
        config.Data.totalUnits = -1
        config.Data.outLFNDirBase = '/store/user/sdonato/' + name
        config.Data.publication = True
        config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
    #    config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
        config.Data.inputDataset = dataset
        config.Data.publishDataName = name+"_"+dataset.split('/')[1]
        
        config.section_("Site")
        config.Site.storageSite = "T2_IT_Pisa"
        submit(config)
    
#    name = 'Skim_13TeV_Mu20_DCS_V2_off_DiJetC30'
#    config.General.workArea = 'crab_'+name
#    config.General.transferLogs = True
#    config.JobType.pluginName = 'Analysis'
#    config.JobType.psetName = 'SkimMu.py'
#    config.Data.inputDBS = 'global'
#    config.Data.splitting = 'FileBased'
#    config.Data.publication = False
#    config.Site.storageSite = 'T2_IT_Pisa'
#    
#    datasets=[
#    '/SingleMuon/Run2015B-PromptReco-v1/MINIAOD',
#    ]
#    for dataset in datasets:
#        config.General.requestName = name+"_"+dataset.split('/')[1]
#        config.Data.inputDataset = dataset
#        config.Data.unitsPerJob = 10
#        config.Data.totalUnits = -1
#        config.Data.publishDataName = name+"_"+dataset.split('/')[1]
#        config.Data.outLFNDirBase = '/store/user/sdonato/' + name # or '/store/group/<subdir>'
#        config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/DCSOnly/json_DCSONLY_Run2015B.txt'
#        submit(config)
#        
#        
#        
        
