from launchNtuple import launchNtuple

maxevents=1000000000000000
fileOutput = 'ntupleEle27.root'
filesInput=[
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_1.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_2.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_3.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_4.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_5.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_6.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30/SingleElectron/Skim_13TeV_Ele27_DCS_V2_off_DiJetC30_SingleElectron/150728_153257/0000/SkimEle27_7.root"
]
launchNtuple(fileOutput,filesInput,maxevents)
