#for file in TRK_newTracking MUO_newTracking MUO_updateTkMu MUO_updateOpenMu MUO_updateNoVtx MUO_newIO MUO_newOI BTV_noCalo_roiPF_DeepCSV BTV_noCalo_roiPF_DeepJet BTV_roiCalo_roiPF_DeepCSV BTV_roiCalo_roiPF_DeepJet BTV_roiCalo_globalPF_DeepCSV BTV_roiCalo_globalPF_DeepJet BTV_globalCalo_globalPF_DeepCSV BTV_globalCalo_globalPF_DeepJet MUO_useGEM MUO_newReco combined ;

for file in combined nothing;
do 
    config="/users/sdonato/CMSSW_12_2_0_GRunV5/"$file"/V3"
    hltGetConfiguration $config > $file.py && edmConfigDump $file.py > $file_dump.py &
done;

