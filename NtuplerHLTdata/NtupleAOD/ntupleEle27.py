from launchNtupleFromAODTest import launchNtupleFromAOD

maxevents=100000000000000000000000
fileOutput = 'ntupleTest.root'
filesInput=[
"file:///scratch/sdonato/Data13TeV/ZvvHbb/CMSSW_7_4_7/src/NtupleAOD/MET_Run251643_AOD.root",
#"file:///scratch/sdonato/Data13TeV/ZvvHbb/CMSSW_7_4_7/src/NtupleAOD/BTagCSVRun2015B251883AOD.root",
#"file:///scratch/sdonato/Data13TeV/ZvvHbb/CMSSW_7_4_7/src/NtupleAOD/TT_Spring15_AODSIM.root",

#"/scratch/sdonato/./PVsorting/CMSSW_7_4_0_pre6/src/WToENu_PU40bx25_AODSIM.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_2.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_3.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_4.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_5.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_6.root",
#"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_7.root"
]
launchNtupleFromAOD(fileOutput,filesInput,maxevents)
