from launchNtuple import launchNtuple

maxevents=1000000000000000
fileOutput = 'ntupleMu20.root'
filesInput=[
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_1.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_2.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_3.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_4.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_5.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30/SingleMuon/Skim_13TeV_Mu20_DCS_V2_off_DiJetC30_SingleMuon/150728_153506/0000/SkimEle27_6.root",
]
launchNtuple(fileOutput,filesInput,maxevents)
