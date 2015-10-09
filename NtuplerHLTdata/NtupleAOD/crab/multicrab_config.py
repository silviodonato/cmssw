datasets=[
    '/BTagCSV/Run2015D-PromptReco-v3/AOD',
    '/BTagCSV/Run2015D-PromptReco-v4/AOD',
    '/BTagMu/Run2015D-PromptReco-v3/AOD',
    '/BTagMu/Run2015D-PromptReco-v4/AOD',
    '/DisplacedJet/Run2015D-PromptReco-v3/AOD',
    '/DisplacedJet/Run2015D-PromptReco-v4/AOD',
    '/DoubleEG/Run2015D-PromptReco-v3/AOD',
    '/DoubleEG/Run2015D-PromptReco-v4/AOD',
    '/DoubleEG_0T/Run2015D-PromptReco-v3/AOD',
    '/DoubleMuon/Run2015D-PromptReco-v3/AOD',
    '/DoubleMuon/Run2015D-PromptReco-v4/AOD',
    '/HLTPhysics/Run2015D-PromptReco-v3/AOD',
    '/HLTPhysics/Run2015D-PromptReco-v4/AOD',
    '/HTMHT/Run2015D-PromptReco-v3/AOD',
    '/HTMHT/Run2015D-PromptReco-v4/AOD',
    '/JetHT/Run2015D-PromptReco-v3/AOD',
    '/JetHT/Run2015D-PromptReco-v4/AOD',
    '/MET/Run2015D-PromptReco-v3/AOD',
    '/MET/Run2015D-PromptReco-v4/AOD',
    '/SingleElectron/Run2015D-PromptReco-v3/AOD',
    '/SingleElectron/Run2015D-PromptReco-v4/AOD',
    '/SingleElectron_0T/Run2015D-PromptReco-v3/AOD',
    '/SingleMuon/Run2015D-PromptReco-v3/AOD',
    '/SingleMuon/Run2015D-PromptReco-v4/AOD',
    '/SinglePhoton/Run2015D-PromptReco-v3/AOD',
    '/SinglePhoton/Run2015D-PromptReco-v4/AOD',
    '/Tau/Run2015D-PromptReco-v3/AOD',
    '/Tau/Run2015D-PromptReco-v4/AOD',
    '/ZeroBias/Run2015D-PromptReco-v3/AOD',
    '/ZeroBias/Run2015D-PromptReco-v4/AOD',
    
    '/ZH_HToBB_ZToLL_M125_13TeV_amcatnloFXFX_madspin_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/ZH_HToBB_ZToNuNu_M125_13TeV_amcatnloFXFX_madspin_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/WH_HToBB_WToLNu_M125_13TeV_amcatnloFXFX_madspin_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
    '/TT_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/AODSIM',
]
if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand
    
    def submit(config):
        res = crabCommand('submit', config = config)
    
    from CRABClient.UserUtilities import config
    config = config()
    for dataset in datasets:
        name = 'triggerNtupleAOD_FWLite_v21'
        config.section_("General")
        config.General.workArea = 'crab_'+name
        config.General.transferLogs=True
        config.General.requestName = name+"_"+dataset.replace('/',"_")
#        config.General.requestName = name+"_"+dataset.split('/')[1]

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
        config.Data.unitsPerJob = 1 ##FIXME: use 20
        config.Data.totalUnits = 1 ##FIXME: use -1
        config.Data.outLFNDirBase = '/store/user/sdonato/' + name
        config.Data.publication = True
        config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
    #    config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
        config.Data.inputDataset = dataset
        config.Data.publishDataName = name+"_"+dataset.split('/')[1]
        
        config.section_("Site")
        config.Site.storageSite = "T2_IT_Pisa"
        submit(config)
    
