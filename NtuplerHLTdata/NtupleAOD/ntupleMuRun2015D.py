from launchNtupleFromAOD import launchNtupleFromAOD

maxevents=100000000000000000000000
fileOutput = 'ntupleTest.root'
filesInput=[
"file:///scratch/sdonato/Files/Run2015D_SingleMuon_AOD_258_428_02163E01440B.root",

]
launchNtupleFromAOD(fileOutput,filesInput,maxevents)
