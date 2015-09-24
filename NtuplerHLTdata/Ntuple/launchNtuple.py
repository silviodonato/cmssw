def launchNtuple(fileOutput,filesInput,maxevents):
    import ROOT
    from array import array
    from math import sqrt, pi, log10, log, exp
    # load FWlite python libraries
    from DataFormats.FWLite import Handle, Events

    pt_min=20
    eta_max=2.4
    NHFmax=0.9
    NEMFmax=0.99
    CHFmin=0.
    MUFmax=0.8
    CEMFmax=0.99
    NumConstMin=1
    CHMmin=0

    f = ROOT.TFile(fileOutput,"recreate")
    tree = ROOT.TTree("tree","tree")

    maxJets = 10
    triggerNames = [
    'HLT_IsoMu20_v2', #0
    'HLT_Ele27_eta2p1_WPLoose_Gsf_v1', #1
    'HLT_DiPFJetAve60_v1', #2
    'HLT_PFMET90_PFMHT90_IDTight_v1', #3
    'HLT_PFMET100_PFMHT100_IDTight_v1', #4
    'HLT_PFMET110_PFMHT110_IDTight_v1', #5
    'HLT_PFMET120_PFMHT120_IDTight_v1', #6
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV0p72_v1', #7
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_v1', #8
    'HLT_PFMET120_NoiseCleaned_BTagCSV0p72_v2', #9
    'HLT_PFMET120_NoiseCleaned_Mu5_v2', #10
    'HLT_PFMET170_NoiseCleaned_v2', #11
    'HLT_PFMET170_v1', #12
    'HLT_Ele27_WPLoose_Gsf_WHbbBoost_v1', #12
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq200_v2', #13
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq460_v2', #14
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq240_v2', #15
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq500_v2', #16
    'HLT_QuadPFJet_VBF_v2', #17
    'HLT_L1_TripleJet_VBF_v1', #18
    'HLT_QuadJet45_TripleBTagCSV0p67_v2',  #19
    'HLT_QuadJet45_DoubleBTagCSV0p67_v2',  #20
    'HLT_DoubleJet90_Double30_TripleBTagCSV0p67_v2',  #21
    'HLT_DoubleJet90_Double30_DoubleBTagCSV0p67_v2',  #22
    'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v2',
    'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v2',
    ]

    nVertices = array( 'i', [ 0 ] )
    tree.Branch( 'nVertices', nVertices, 'nVertices/I' )
    run = array( 'i', [ 0 ] )
    tree.Branch( 'run', run, 'run/I' )
    eventNumber = array( 'i', [ 0 ] )
    tree.Branch( 'event', eventNumber, 'event/I' )


    offMet = array( 'f', [ 0 ] )
    tree.Branch( 'offMet', offMet, 'offMet/F' )
    offMet_phi = array( 'f', [ 0 ] )
    tree.Branch( 'offMet_phi', offMet_phi, 'offMet_phi/F' )
    offMet_sig = array( 'f', [ 0 ] )
    tree.Branch( 'offMet_sig', offMet_sig, 'offMet_sig/F' )
    offMet_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'offMet_sumet', offMet_sumet, 'offMet_sumet/F' )

    offMht = array( 'f', [ 0 ] )
    tree.Branch( 'offMht', offMht, 'offMht/F' )
    offMht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'offMht_phi', offMht_phi, 'offMht_phi/F' )
    offMht_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'offMht_sumet', offMht_sumet, 'offMht_sumet/F' )

    offPUPPIMht = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMht', offPUPPIMht, 'offPUPPIMht/F' )
    offPUPPIMht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMht_phi', offPUPPIMht_phi, 'offPUPPIMht_phi/F' )
    offPUPPIMht_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMht_sumet', offPUPPIMht_sumet, 'offPUPPIMht_sumet/F' )

    offJet_num = array( 'i', [ 0 ] )
    tree.Branch( 'offJet_num', offJet_num, 'offJet_num/I' )
    offJet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_pt', offJet_pt, 'offJet_pt[offJet_num]/F' )
    offJet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_eta', offJet_eta, 'offJet_eta[offJet_num]/F' )
    offJet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_phi', offJet_phi, 'offJet_phi[offJet_num]F' )
    offJet_csv = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_csv', offJet_csv, 'offJet_csv[offJet_num]F' )

    offElectron_num = array( 'i', [ 0 ] )
    tree.Branch( 'offElectron_num', offElectron_num, 'offElectron_num/I' )
    offElectron_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_pt', offElectron_pt, 'offElectron_pt[offElectron_num]/F' )
    offElectron_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_eta', offElectron_eta, 'offElectron_eta[offElectron_num]/F' )
    offElectron_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_phi', offElectron_phi, 'offElectron_phi[offElectron_num]F' )
    offElectron_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_iso', offElectron_iso, 'offElectron_iso[offElectron_num]F' )

    offMuon_num = array( 'i', [ 0 ] )
    tree.Branch( 'offMuon_num', offMuon_num, 'offMuon_num/I' )
    offMuon_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_pt', offMuon_pt, 'offMuon_pt[offMuon_num]/F' )
    offMuon_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_eta', offMuon_eta, 'offMuon_eta[offMuon_num]/F' )
    offMuon_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_phi', offMuon_phi, 'offMuon_phi[offMuon_num]F' )
    offMuon_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_iso', offMuon_iso, 'offMuon_iso[offMuon_num]F' )

    offPhoton_num = array( 'i', [ 0 ] )
    tree.Branch( 'offPhoton_num', offPhoton_num, 'offPhoton_num/I' )
    offPhoton_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_pt', offPhoton_pt, 'offPhoton_pt[offPhoton_num]/F' )
    offPhoton_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_eta', offPhoton_eta, 'offPhoton_eta[offPhoton_num]/F' )
    offPhoton_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_phi', offPhoton_phi, 'offPhoton_phi[offPhoton_num]F' )
    offPhoton_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_iso', offPhoton_iso, 'offPhoton_iso[offPhoton_num]F' )


    offPUPPIMet = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMet', offPUPPIMet, 'offPUPPIMet/F' )
    offPUPPIMet_phi = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMet_phi', offPUPPIMet_phi, 'offPUPPIMet_phi/F' )
    offPUPPIMet_sig = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMet_sig', offPUPPIMet_sig, 'offPUPPIMet_sig/F' )
    offPUPPIMet_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'offPUPPIMet_sumet', offPUPPIMet_sumet, 'offPUPPIMet_sumet/F' )

    offPUPPIJet_num = array( 'i', [ 0 ] )
    tree.Branch( 'offPUPPIJet_num', offPUPPIJet_num, 'offPUPPIJet_num/I' )
    offPUPPIJet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_pt', offPUPPIJet_pt, 'offPUPPIJet_pt[offPUPPIJet_num]/F' )
    offPUPPIJet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_eta', offPUPPIJet_eta, 'offPUPPIJet_eta[offPUPPIJet_num]/F' )
    offPUPPIJet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_phi', offPUPPIJet_phi, 'offPUPPIJet_phi[offPUPPIJet_num]F' )
    offPUPPIJet_csv = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_csv', offPUPPIJet_csv, 'offPUPPIJet_csv[offPUPPIJet_num]F' )

    nTriggers = len(triggerNames)
    triggers =  array( 'i', nTriggers * [ 0 ] )
    tree.Branch( 'triggers', triggers, 'triggers['+str(nTriggers)+']/I' )
    
    def checkTriggerIndex(name,index, names):
        if index>=names.size():
            for tr in names: print tr
            print
            print name," not found!"
            print

    triggerBits, triggerBitLabel = Handle("edm::TriggerResults"), ("TriggerResults::HLT")
    triggerObjects, triggerObjectLabel  = Handle("std::vector<pat::TriggerObjectStandAlone>"), "selectedPatTrigger"
    triggerPrescales, triggerPrescaleLabel  = Handle("pat::PackedTriggerPrescales"), "patTrigger"
    patJets, patJetLabel = Handle("vector<pat::Jet>"), ("slimmedJets")
    patMets, patMetLabel = Handle("vector<pat::MET>"), ("slimmedMETs")
    patPUPPIJets, patPUPPIJetLabel = Handle("vector<pat::Jet>"), ("slimmedJetsPuppi")
    patPUPPIMets, patPUPPIMetLabel = Handle("vector<pat::MET>"), ("slimmedMETsPuppi")
    recoVertexs, recoVertexLabel = Handle("vector<reco::Vertex>"), ("offlineSlimmedPrimaryVertices")
    patElectrons, patElectronLabel = Handle("vector<pat::Electron>"), ("slimmedElectrons")
    patMuons, patMuonLabel = Handle("vector<pat::Muon>"), ("slimmedMuons")
    patPhotons, patPhotonLabel = Handle("vector<pat::Photon>"), ("slimmedPhotons")

    # open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
    events = Events (filesInput)

    #events.to(1)
    #event = events.object()

    for iev,event in enumerate(events):
        if iev%1000==0: print "iev=",iev
        if iev>maxevents: break
        event.getByLabel(triggerBitLabel, triggerBits)
        event.getByLabel(triggerObjectLabel, triggerObjects)
        event.getByLabel(triggerPrescaleLabel, triggerPrescales)
        event.getByLabel(patJetLabel, patJets)
        event.getByLabel(patMetLabel, patMets)
        event.getByLabel(patPUPPIJetLabel, patPUPPIJets)
        event.getByLabel(patPUPPIMetLabel, patPUPPIMets)
        event.getByLabel(recoVertexLabel, recoVertexs)
        event.getByLabel(patElectronLabel, patElectrons)
        event.getByLabel(patMuonLabel, patMuons)
        event.getByLabel(patPhotonLabel, patPhotons)
        
        nVertices[0] = recoVertexs.product().size()
        run[0] = event.eventAuxiliary().run()
        eventNumber[0] = event.eventAuxiliary().event()
        
        i=0
        for jet in patJets.product():
            if jet.pt()<20: continue
            if i<maxJets:
                offJet_pt[i] = jet.pt()
                offJet_eta[i] = jet.eta()
                offJet_phi[i] = jet.phi()
                offJet_csv[i] = max(0.,jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"))
                offJet_num[0] = i + 1
                i+=1

        i=0
        for muon in patMuons.product():
            if muon.pt()<10: continue
            if i<maxJets:
                offMuon_pt[i] = muon.pt()
                offMuon_eta[i] = muon.eta()
                offMuon_phi[i] = muon.phi()
                offMuon_iso[i] = 0
                offMuon_num[0] = i + 1
                i+=1

        i=0
        for electron in patElectrons.product():
            if electron.pt()<10: continue
            if i<maxJets:
                offElectron_pt[i] = electron.pt()
                offElectron_eta[i] = electron.eta()
                offElectron_phi[i] = electron.phi()
                offElectron_iso[i] = 0
                offElectron_num[0] = i + 1
                i+=1

        i=0
        for photon in patPhotons.product():
            if photon.pt()<10: continue
            if i<maxJets:
                offPhoton_pt[i] = photon.pt()
                offPhoton_eta[i] = photon.eta()
                offPhoton_phi[i] = photon.phi()
                offPhoton_iso[i] = 0
                offPhoton_num[0] = i + 1
                i+=1

                
        offMet[0] = patMets.product().begin().pt()
        offMet_phi[0] = patMets.product().begin().phi()
        offMet_sumet[0] = patMets.product().begin().sumEt()
        offMet_sig[0] = patMets.product().begin().significance()

        i=0
        for jet in patPUPPIJets.product():
            if jet.pt()<20: continue
            if i<maxJets:
                offPUPPIJet_pt[i] = jet.pt()
                offPUPPIJet_eta[i] = jet.eta()
                offPUPPIJet_phi[i] = jet.phi()
                offPUPPIJet_csv[i] = max(0.,jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"))
                offPUPPIJet_num[0] = i + 1
                i+=1
                
        offPUPPIMet[0] = patPUPPIMets.product().begin().pt()
        offPUPPIMet_phi[0] = patPUPPIMets.product().begin().phi()
        offPUPPIMet_sumet[0] = patPUPPIMets.product().begin().sumEt()
        offPUPPIMet_sig[0] = patPUPPIMets.product().begin().significance()
        
        
        MHT2D = ROOT.TVector2()
        jet2D = ROOT.TVector2()
        offMht_sumet[0]=0
        for jet in patJets.product():
            if jet.pt()>pt_min and abs(jet.eta())<eta_max \
            and jet.neutralHadronEnergyFraction()<NHFmax \
            and jet.neutralEmEnergyFraction()<NEMFmax \
            and jet.chargedHadronEnergyFraction()>CHFmin \
            and jet.muonEnergyFraction()<MUFmax \
            and jet.chargedEmEnergyFraction()<CEMFmax \
            and jet.chargedMultiplicity()+jet.neutralMultiplicity()>NumConstMin \
            and jet.chargedMultiplicity()>CHMmin :
                jet2D.SetMagPhi(jet.pt(),jet.phi())
                MHT2D = MHT2D - jet2D
                offMht_sumet[0]+=jet.pt()
        
        offMht[0] = MHT2D.Mod()
        offMht_phi[0] = MHT2D.Phi()
        
        MHTPUPPI2D = ROOT.TVector2()
        jetPUPPI2D = ROOT.TVector2()
        offPUPPIMht_sumet[0] = 0
        for jet in patPUPPIJets.product():
            if jet.pt()>pt_min and abs(jet.eta())<eta_max \
            and jet.neutralHadronEnergyFraction()<NHFmax \
            and jet.neutralEmEnergyFraction()<NEMFmax \
            and jet.chargedHadronEnergyFraction()>CHFmin \
            and jet.muonEnergyFraction()<MUFmax \
            and jet.chargedEmEnergyFraction()<CEMFmax \
            and jet.chargedMultiplicity()+jet.neutralMultiplicity()>NumConstMin \
            and jet.chargedMultiplicity()>CHMmin :
                jetPUPPI2D.SetMagPhi(jet.pt(),jet.phi())
                MHTPUPPI2D = MHTPUPPI2D - jetPUPPI2D
                offPUPPIMht_sumet[0] += jet.pt()
        
        offPUPPIMht[0] = MHTPUPPI2D.Mod()
        offPUPPIMht_phi[0] = MHTPUPPI2D.Phi()
        
        names = event.object().triggerNames(triggerBits.product())
        for i,triggerName in enumerate(triggerNames):
            index = names.triggerIndex(triggerName)
            checkTriggerIndex(triggerName,index,names.triggerNames())
            triggers[i] = triggerBits.product().accept(index)
        tree.Fill()

    f.Write()
    f.Close()


maxevents=10
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
