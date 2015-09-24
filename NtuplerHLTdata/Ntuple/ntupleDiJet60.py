from launchNtuple import launchNtuple

maxevents=1000000000000000
fileOutput = 'ntupleDiJet60.root'
filesInput=[
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_1.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_2.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_3.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_4.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_5.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_6.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_7.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_8.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_9.root",
"/gpfs/ddn/srm/cms/store/user/sdonato/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30/JetHT/Skim_13TeV_DiJetAve60_DCS_V3_off_DiJetC30_JetHT/150729_125901/0000/SkimDiJet60_10.root",
]
launchNtuple(fileOutput,filesInput,maxevents)
