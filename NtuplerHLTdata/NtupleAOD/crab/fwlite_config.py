from math import sqrt, pi, log10, log, exp

def goodEvent(run,lumi):
    JSONlist={"251244": [[85, 86], [88, 93], [96, 121], [123, 156], [158, 428], [430, 442]],
     "251251": [[1, 31], [33, 97], [99, 167]],
     "251252": [[1, 283], [285, 505], [507, 554]],
     "251561": [[1, 94]],
     "251562": [[1, 439], [443, 691]],
     "251643": [[1, 216], [222, 606]],
     "251721": [[21, 36]],
     "251883": [[56, 56], [58, 60], [62, 144], [156, 437]]}

    if str(run) in JSONlist.keys():
        for rg in JSONlist[str(run)]:
            if len(rg) ==2:
                if lumi>=rg[0] and lumi<=rg[1]:
                    return True
    
    return False

def checkTriggerIndex(name,index, names):
    if not 'firstTriggerError' in globals():
        global firstTriggerError
        firstTriggerError = True
    if index>=names.size():
        if firstTriggerError:
            for tr in names: print tr
            print
            print name," not found!"
            print
            firstTriggerError = False
            return False
        else:
            return False
    else:
        return True

def deltaPhi(phi1, phi2):
  PHI = abs(phi1-phi2)
  if PHI<=pi:
      return PHI
  else:
      return 2*pi-PHI

def deltaR(eta1, phi1, eta2, phi2):
  deta = eta1-eta2
  dphi = deltaPhi(phi1,phi2)
  return sqrt(deta*deta + dphi*dphi)

def matching(eta,phi,offJet_eta,offJet_phi,offJet_num):
    dRMin = 99.
    index = -1
    for i in range(0,offJet_num):
        dR = deltaR(eta,phi,offJet_eta[i],offJet_phi[i])
        if dR < 0.5 and dR < dRMin:
            dRMin = dR
            index = i
    
    return index

def getCollectionKeys(triggerEvent,inputTag):
    collectionKeys = []
    collectionIndex = triggerEvent.collectionIndex(inputTag)
    if collectionIndex<triggerEvent.sizeCollections():
        start = 0
        if collectionIndex>0: start = triggerEvent.collectionKey(collectionIndex-1)
        stop = triggerEvent.collectionKey(collectionIndex)
        collectionKeys = range(start, stop)
        del start
        del stop
    
    del collectionIndex
    return collectionKeys

def getMET(triggerEvent,inputTag):
    collectionKeys = getCollectionKeys(triggerEvent,inputTag)
    trigObjColl = triggerEvent.getObjects()
    if len(collectionKeys)>0:
        met = trigObjColl[collectionKeys[0]]
        return met.pt(), met.phi()
    else:
        return 0,0
    del trigObjColl
    del collectionKeys


def getFilterKeys(triggerEvent,inputTag):
    filterKeys = []
    filterIndex = triggerEvent.filterIndex(inputTag)
    if filterIndex<triggerEvent.sizeFilters():
        filterKeys = triggerEvent.filterKeys(filterIndex)
    
    del filterIndex
    return filterKeys


def launchNtupleFromAOD(fileOutput,filesInput,maxevents):
    print "fileOutput=",fileOutput
    print
    print "filesInput=",filesInput
    print
    print "maxevents=",maxevents
    print

    import ROOT
    import itertools
    import resource
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
    
    maxJets = 50
    triggerNamesMC = [
    'HLT_AK8PFJet360_TrimMass30_v1',
    'HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v1',
    'HLT_CaloJet500_NoJetID_v1',
    'HLT_Dimuon13_PsiPrime_v1',
    'HLT_Dimuon13_Upsilon_v1',
    'HLT_Dimuon20_Jpsi_v1',
    'HLT_DoubleEle24_22_eta2p1_WP75_Gsf_v1',
    'HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v1',
    'HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v1',
    'HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_DoubleMu33NoFiltersNoVtx_v1',
    'HLT_DoubleMu38NoFiltersNoVtx_v1',
    'HLT_DoubleMu23NoFiltersNoVtxDisplaced_v1',
    'HLT_DoubleMu28NoFiltersNoVtxDisplaced_v1',
    'HLT_DoubleMu4_3_Bs_v1',
    'HLT_DoubleMu4_3_Jpsi_Displaced_v1',
    'HLT_DoubleMu4_JpsiTrk_Displaced_v1',
    'HLT_DoubleMu4_LowMassNonResonantTrk_Displaced_v1',
    'HLT_DoubleMu4_PsiPrimeTrk_Displaced_v1',
    'HLT_Mu7p5_L2Mu2_Jpsi_v1',
    'HLT_Mu7p5_L2Mu2_Upsilon_v1',
    'HLT_Mu7p5_Track2_Jpsi_v1',
    'HLT_Mu7p5_Track3p5_Jpsi_v1',
    'HLT_Mu7p5_Track7_Jpsi_v1',
    'HLT_Mu7p5_Track2_Upsilon_v1',
    'HLT_Mu7p5_Track3p5_Upsilon_v1',
    'HLT_Mu7p5_Track7_Upsilon_v1',
    'HLT_Dimuon0er16_Jpsi_NoOS_NoVertexing_v1',
    'HLT_Dimuon0er16_Jpsi_NoVertexing_v1',
    'HLT_Dimuon6_Jpsi_NoVertexing_v1',
    'HLT_DoublePhoton85_v1',
    'HLT_Ele20WP60_Ele8_Mass55_v1',
    'HLT_Ele22_eta2p1_WP75_Gsf_v1',
    'HLT_Ele22_eta2p1_WP75_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele25WP60_SC4_Mass55_v1',
    'HLT_Ele27_WP85_Gsf_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_CentralPFJet30_BTagCSV07_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_TriCentralPFJet30_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_TriCentralPFJet50_40_30_v1',
    'HLT_Ele27_eta2p1_WP75_Gsf_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_CentralPFJet30_BTagCSV07_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_TriCentralPFJet30_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_TriCentralPFJet50_40_30_v1',
    'HLT_Ele32_eta2p1_WP75_Gsf_v1',
    'HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v1',
    'HLT_Ele105_CaloIdVT_GsfTrkIdT_v1',
    'HLT_Mu16_eta2p1_CaloMET30_v1',
    'HLT_IsoMu16_eta2p1_CaloMET30_v1',
    'HLT_IsoMu16_eta2p1_CaloMET30_LooseIsoPFTau50_Trk30_eta2p1_v1',
    'HLT_IsoMu17_eta2p1_v1',
    'HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v1',
    'HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1_v1',
    'HLT_IsoMu17_eta2p1_MediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_DoubleIsoMu17_eta2p1_v1',
    'HLT_IsoMu24_eta2p1_LooseIsoPFTau20_v1',
    'HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v1',
    'HLT_IsoMu20_eta2p1_TriCentralPFJet30_v1',
    'HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v1',
    'HLT_IsoMu20_v1',
    'HLT_IsoMu20_eta2p1_v1',
    'HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v1',
    'HLT_IsoMu24_eta2p1_TriCentralPFJet30_v1',
    'HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v1',
    'HLT_IsoMu24_eta2p1_v1',
    'HLT_IsoMu27_v1',
    'HLT_IsoTkMu20_v1',
    'HLT_IsoTkMu20_eta2p1_v1',
    'HLT_IsoTkMu24_eta2p1_v1',
    'HLT_IsoTkMu27_v1',
    'HLT_JetE30_NoBPTX3BX_NoHalo_v1',
    'HLT_JetE30_NoBPTX_v1',
    'HLT_JetE50_NoBPTX3BX_NoHalo_v1',
    'HLT_JetE70_NoBPTX3BX_NoHalo_v1',
    'HLT_L1SingleMuOpen_v1',
    'HLT_L1SingleMuOpen_DT_v1',
    'HLT_L1Tech_DT_GlobalOR_v1',
    'HLT_L2DoubleMu23_NoVertex_v1',
    'HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10_v1',
    'HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10_v1',
    'HLT_L2Mu10_NoVertex_NoBPTX3BX_NoHalo_v1',
    'HLT_L2Mu10_NoVertex_NoBPTX_v1',
    'HLT_L2Mu35_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1',
    'HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_v1',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120_v1',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80_v1',
    'HLT_Mu17_Mu8_DZ_v1',
    'HLT_Mu17_TkMu8_DZ_v1',
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v1',
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v1',
    'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v1',
    'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v1',
    'HLT_Mu25_TkMu0_dEta18_Onia_v1',
    'HLT_Mu27_TkMu8_v1',
    'HLT_Mu30_TkMu11_v1',
    'HLT_Mu40_TkMu11_v1',
    'HLT_Mu40_eta2p1_PFJet200_PFJet50_v1',
    'HLT_Mu20_v1',
    'HLT_TkMu20_v1',
    'HLT_Mu24_eta2p1_v1',
    'HLT_TkMu24_eta2p1_v1',
    'HLT_Mu27_v1',
    'HLT_TkMu27_v1',
    'HLT_Mu50_v1',
    'HLT_Mu45_eta2p1_v1',
    'HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL_v1',
    'HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL_v1',
    'HLT_Mu28NoFiltersNoVtxDisplaced_Photon28_CaloIdL_v1',
    'HLT_Mu33NoFiltersNoVtxDisplaced_Photon33_CaloIdL_v1',
    'HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL_v1',
    'HLT_DoubleMu18NoFiltersNoVtx_v1',
    'HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Tight_v1',
    'HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Loose_v1',
    'HLT_Mu28NoFiltersNoVtx_DisplacedJet40_Loose_v1',
    'HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Tight_v1',
    'HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Loose_v1',
    'HLT_Mu38NoFiltersNoVtx_DisplacedJet60_Loose_v1',
    'HLT_Mu28NoFiltersNoVtx_CentralCaloJet40_v1',
    'HLT_PFHT350_PFMET120_NoiseCleaned_v1',
    'HLT_PFHT550_4Jet_v1',
    'HLT_PFHT650_4Jet_v1',
    'HLT_PFHT750_4Jet_v1',
    'HLT_PFHT350_v1',
    'HLT_PFHT600_v1',
    'HLT_PFHT650_v1',
    'HLT_PFHT900_v1',
    'HLT_PFJet40_v1',
    'HLT_PFJet60_v1',
    'HLT_PFJet80_v1',
    'HLT_PFJet140_v1',
    'HLT_PFJet200_v1',
    'HLT_PFJet260_v1',
    'HLT_PFJet320_v1',
    'HLT_PFJet400_v1',
    'HLT_PFJet450_v1',
    'HLT_PFJet500_v1',
    'HLT_DiPFJetAve30_HFJEC_v1',
    'HLT_DiPFJetAve60_HFJEC_v1',
    'HLT_DiPFJetAve80_HFJEC_v1',
    'HLT_DiPFJetAve100_HFJEC_v1',
    'HLT_DiPFJetAve160_HFJEC_v1',
    'HLT_DiPFJetAve220_HFJEC_v1',
    'HLT_DiPFJetAve300_HFJEC_v1',
    'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140_v1',
    'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu80_v1',
    'HLT_DiCentralPFJet70_PFMET120_NoiseCleaned_v1',
    'HLT_HT200_v1',
    'HLT_HT250_v1',
    'HLT_HT300_v1',
    'HLT_HT350_v1',
    'HLT_HT400_v1',
    'HLT_PFHT200_DiPFJet90_PFAlphaT0p57_v1',
    'HLT_PFHT250_DiPFJet90_PFAlphaT0p55_v1',
    'HLT_PFHT300_DiPFJet90_PFAlphaT0p53_v1',
    'HLT_PFHT350_DiPFJet90_PFAlphaT0p52_v1',
    'HLT_PFHT400_DiPFJet90_PFAlphaT0p51_v1',
    'HLT_MET75_IsoTrk50_v1',
    'HLT_MET90_IsoTrk50_v1',
    'HLT_PFMET120_NoiseCleaned_BTagCSV07_v1',
    'HLT_PFMET120_NoiseCleaned_Mu5_v1',
    'HLT_PFMET170_NoiseCleaned_v1',
    'HLT_PFMET90_PFMHT90_IDLoose_v1',
    'HLT_PFMET100_PFMHT100_IDLoose_v1',
    'HLT_PFMET110_PFMHT110_IDLoose_v1',
    'HLT_PFMET120_PFMHT120_IDLoose_v1',
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDLoose_BTagCSV0p7_v1',
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDLoose_v1',
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq200_v1',
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq460_v1',
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq240_v1',
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq500_v1',
    'HLT_QuadPFJet_VBF_v1',
    'HLT_L1_TripleJet_VBF_v1',
    'HLT_QuadJet45_TripleCSV0p5_v1',
    'HLT_QuadJet45_DoubleCSV0p5_v1',
    'HLT_DoubleJet90_Double30_TripleCSV0p5_v1',
    'HLT_DoubleJet90_Double30_DoubleCSV0p5_v1',
    'HLT_Photon135_PFMET100_NoiseCleaned_v1',
    'HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Photon250_NoHE_v1',
    'HLT_Photon300_NoHE_v1',
    'HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60_v1',
    'HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v1',
    'HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40_v1',
    'HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_VBF_v1',
    'HLT_Mu8_TrkIsoVVL_v1',
    'HLT_Mu17_TrkIsoVVL_v1',
    'HLT_Mu24_TrkIsoVVL_v1',
    'HLT_Mu34_TrkIsoVVL_v1',
    'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v1',
    'HLT_Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30_v1',
    'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v1',
    'HLT_Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30_v1',
    'HLT_BTagMu_DiJet20_Mu5_v1',
    'HLT_BTagMu_DiJet40_Mu5_v1',
    'HLT_BTagMu_DiJet70_Mu5_v1',
    'HLT_BTagMu_DiJet110_Mu5_v1',
    'HLT_BTagMu_Jet300_Mu5_v1',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v1',
    'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v1',
    'HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v1',
    'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL_v1',
    'HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v1',
    'HLT_Mu12_Photon25_CaloIdL_v1',
    'HLT_Mu12_Photon25_CaloIdL_L1ISO_v1',
    'HLT_Mu12_Photon25_CaloIdL_L1OR_v1',
    'HLT_Mu17_Photon30_CaloIdL_L1ISO_v1',
    'HLT_Mu17_Photon35_CaloIdL_L1ISO_v1',
    'HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v1',
    'HLT_TripleMu_12_10_5_v1',
    'HLT_Mu3er_PFHT140_PFMET125_NoiseCleaned_v1',
    'HLT_Mu6_PFHT200_PFMET100_NoiseCleaned_BTagCSV07_v1',
    'HLT_Mu6_PFHT200_PFMET125_NoiseCleaned_v1',
    'HLT_Mu14er_PFMET120_NoiseCleaned_v1',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV0p41_v1',
    'HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v1',
    'HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v1',
    'HLT_Photon22_v1',
    'HLT_Photon30_v1',
    'HLT_Photon36_v1',
    'HLT_Photon50_v1',
    'HLT_Photon75_v1',
    'HLT_Photon90_v1',
    'HLT_Photon120_v1',
    'HLT_Photon175_v1',
    'HLT_Photon165_HE10_v1',
    'HLT_Photon22_R9Id90_HE10_IsoM_v1',
    'HLT_Photon30_R9Id90_HE10_IsoM_v1',
    'HLT_Photon36_R9Id90_HE10_IsoM_v1',
    'HLT_Photon50_R9Id90_HE10_IsoM_v1',
    'HLT_Photon75_R9Id90_HE10_IsoM_v1',
    'HLT_Photon90_R9Id90_HE10_IsoM_v1',
    'HLT_Photon120_R9Id90_HE10_IsoM_v1',
    'HLT_Photon165_R9Id90_HE10_IsoM_v1',
    'HLT_Diphoton44_28_R9Id85_OR_Iso50T80LCaloId24b40e_AND_HE10P1_R9Id50b80e_v1',
    'HLT_Diphoton28_14_R9Id85_OR_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_Mass50_Eta_1p5_v1',
    'HLT_Diphoton30_18_R9Id85_OR_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_PixelSeed_Mass70_v1',
    'HLT_Diphoton30_18_R9Id85_OR_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_Mass95_v1',
    'HLT_Diphoton30_18_R9Id85_AND_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_Solid_Mass30_v1',
    'HLT_Diphoton30_18_R9Id85_AND_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_PV_v1',
    'HLT_Diphoton30_18_R9Id85_AND_Iso50T80LCaloId24b40e_AND_HE10P0_R9Id50b80e_DoublePV_v1',
    'HLT_Dimuon0_Jpsi_Muon_v1',
    'HLT_Dimuon0_Upsilon_Muon_v1',
    'HLT_QuadMuon0_Dimuon0_Jpsi_v1',
    'HLT_QuadMuon0_Dimuon0_Upsilon_v1',
    'HLT_Mu17_Mu8_SameSign_v1',
    'HLT_Mu17_Mu8_SameSign_DPhi_v1',
    'HLT_RsqMR260_Rsq0p09_MR200_v1',
    'HLT_RsqMR260_Rsq0p09_MR200_4jet_v1',
    'HLT_RsqMR300_Rsq0p09_MR200_v1',
    'HLT_RsqMR300_Rsq0p09_MR200_4jet_v1',
    'HLT_Rsq0p36_v1',
    'HLT_HT750_DisplacedDijet80_Inclusive_v1',
    'HLT_HT650_DisplacedDijet80_Inclusive_v1',
    'HLT_HT350_DisplacedDijet80_Tight_DisplacedTrack_v1',
    'HLT_HT350_DisplacedDijet40_DisplacedTrack_v1',
    'HLT_HT350_DisplacedDijet80_DisplacedTrack_v1',
    'HLT_HT500_DisplacedDijet40_Inclusive_v1',
    'HLT_HT550_DisplacedDijet40_Inclusive_v1',
    'HLT_VBF_DisplacedJet40_DisplacedTrack_v1',
    'HLT_VBF_DisplacedJet40_TightID_DisplacedTrack_v1',
    'HLT_VBF_DisplacedJet40_Hadronic_v1',
    'HLT_VBF_DisplacedJet40_TightID_Hadronic_v1',
    'HLT_VBF_DisplacedJet40_VTightID_Hadronic_v1',
    'HLT_VBF_DisplacedJet40_VVTightID_Hadronic_v1',
    'HLT_VBF_DisplacedJet40_VTightID_DisplacedTrack_v1',
    'HLT_VBF_DisplacedJet40_VVTightID_DisplacedTrack_v1',
    'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v1',
    'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v1',
    'HLT_MonoCentralPFJet80_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v1',
    'HLT_MonoCentralPFJet80_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v1',
    'HLT_CaloMET200_NoiseCleaned_v1',
    'HLT_Ele27_eta2p1_WP85_Gsf_HT200_v1',
    'HLT_Photon90_CaloIdL_PFHT500_v1',
    'HLT_DoubleMu8_Mass8_PFHT300_v1',
    'HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT300_v1',
    'HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT300_v1',
    'HLT_Mu10_CentralPFJet30_BTagCSV0p5PF_v1',
    'HLT_Ele10_CaloIdM_TrackIdM_CentralPFJet30_BTagCSV0p5PF_v1',
    'HLT_Ele15_IsoVVVL_BTagtop8CSV07_PFHT400_v1',
    'HLT_Ele15_IsoVVVL_PFHT400_PFMET70_v1',
    'HLT_Ele15_IsoVVVL_PFHT600_v1',
    'HLT_Ele15_PFHT300_v1',
    'HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60_v1',
    'HLT_Mu15_IsoVVVL_BTagCSV07_PFHT400_v1',
    'HLT_Mu15_IsoVVVL_PFHT400_PFMET70_v1',
    'HLT_Mu15_IsoVVVL_PFHT600_v1',
    'HLT_Mu15_PFHT300_v1',
    'HLT_Dimuon16_Jpsi_v1',
    'HLT_Dimuon10_Jpsi_Barrel_v1',
    'HLT_Dimuon8_PsiPrime_Barrel_v1',
    'HLT_Dimuon8_Upsilon_Barrel_v1',
    'HLT_Dimuon0_Phi_Barrel_v1',
    'HLT_Mu16_TkMu0_dEta18_Onia_v1',
    'HLT_Mu16_TkMu0_dEta18_Phi_v1',
    'HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx_v1',
    'HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v1',
    'HLT_Mu8_v1',
    'HLT_Mu17_v1',
    'HLT_Mu24_v1',
    'HLT_Mu34_v1',
    'HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v1',
    'HLT_Ele12_CaloIdM_TrackIdM_PFJet30_v1',
    'HLT_Ele18_CaloIdM_TrackIdM_PFJet30_v1',
    'HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v1',
    'HLT_Ele33_CaloIdM_TrackIdM_PFJet30_v1',
    'HLT_PFHT450_SixJet40_PFBTagCSV_v1',
    'HLT_PFHT400_SixJet30_BTagCSV0p5_2PFBTagCSV_v1',
    'HLT_PFHT450_SixJet40_v1',
    'HLT_PFHT400_SixJet30_v1',
    'HLT_ECALHT800_v1',
    'HLT_Physics_v1',
    'HLT_ReducedIterativeTracking_v1',
    'HLT_Random_v1',
    'HLT_ZeroBias_v1',
    'HLT_L1SingleEG5_OR_EG10_OR_EG20_v1',
    'HLT_Activity_Ecal_SC7_v1',
    'HLT_EcalCalibration_v1',
    'HLT_HcalCalibration_v1',
    'HLT_GlobalRunHPDNoise_v1',
    'HLT_L1Tech_HBHEHO_totalOR_v1',
    'HLT_L1Tech_HCAL_HF_single_channel_v1',
    'HLT_HcalNZS_v1',
    'HLT_HcalPhiSym_v1',
    'HLT_HcalUTCA_v1',
    'HLT_PFMETForMC_v1',
    'HLT_AK4PFJetsForMC_v1',
    'HLT_PFHTForMC_v1',
    'HLT_PFMHTForMC_v1',
    'HLT_CaloMETForMC_v1',
    'HLT_CaloMETCleanedForMC_v1',
    'HLT_AK4CaloJetsForMC_v1',
    'HLT_CaloHTForMC_v1',
    'HLT_CaloMHTForMC_v1',
    'HLT_AK8PFJetsForMC_v1',
    'HLT_AK8TrimPFJetsForMC_v1',
    'HLT_AK8PFHTForMC_v1',
    'HLT_AK8CaloHTForMC_v1',
    'HLT_IsoMu_SaveObjects_v1',
    'HLT_IsoTkMu15_SaveObjects_v1',
    'HLT_DoubleMu_TrkIsoVVL_DZ_SaveObjects_v1',
    'HLT_DoubleGlbTrkMu_TrkIsoVVL_DZ_SaveObjects_v1',
    'HLT_DoubleMuNoFiltersNoVtx_SaveObjects_v1',
    ]

    triggerNames = [
    'HLT_AK8PFJet360_TrimMass30_v2',
    'HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v2',
    'HLT_CaloJet500_NoJetID_v2',
    'HLT_Dimuon13_PsiPrime_v1',
    'HLT_Dimuon13_Upsilon_v1',
    'HLT_Dimuon20_Jpsi_v1',
    'HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v1',
    'HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v2',
    'HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v2',
    'HLT_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v2',
    'HLT_DoubleMu33NoFiltersNoVtx_v1',
    'HLT_DoubleMu38NoFiltersNoVtx_v1',
    'HLT_DoubleMu23NoFiltersNoVtxDisplaced_v1',
    'HLT_DoubleMu28NoFiltersNoVtxDisplaced_v1',
    'HLT_DoubleMu4_3_Bs_v1',
    'HLT_DoubleMu4_3_Jpsi_Displaced_v1',
    'HLT_DoubleMu4_JpsiTrk_Displaced_v2',
    'HLT_DoubleMu4_LowMassNonResonantTrk_Displaced_v2',
    'HLT_DoubleMu4_PsiPrimeTrk_Displaced_v2',
    'HLT_Mu7p5_L2Mu2_Jpsi_v1',
    'HLT_Mu7p5_L2Mu2_Upsilon_v1',
    'HLT_Mu7p5_Track2_Jpsi_v2',
    'HLT_Mu7p5_Track3p5_Jpsi_v2',
    'HLT_Mu7p5_Track7_Jpsi_v2',
    'HLT_Mu7p5_Track2_Upsilon_v2',
    'HLT_Mu7p5_Track3p5_Upsilon_v2',
    'HLT_Mu7p5_Track7_Upsilon_v2',
    'HLT_Dimuon0er16_Jpsi_NoOS_NoVertexing_v1',
    'HLT_Dimuon0er16_Jpsi_NoVertexing_v1',
    'HLT_Dimuon6_Jpsi_NoVertexing_v1',
    'HLT_DoublePhoton85_v2',
    'HLT_Ele25WP60_Ele8_Mass55_v2',
    'HLT_Ele25WP60_SC4_Mass55_v2',
    'HLT_Ele22_eta2p1_WPLoose_Gsf_v1',
    'HLT_Ele22_eta2p1_WPTight_Gsf_v1',
    'HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele23_WPLoose_Gsf_v1',
    'HLT_Ele27_WPLoose_Gsf_WHbbBoost_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v1',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_v1',
    'HLT_Ele27_eta2p1_WPTight_Gsf_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_LooseIsoPFTau20_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v1',
    'HLT_Ele32_eta2p1_WPLoose_Gsf_v1',
    'HLT_Ele32_eta2p1_WPTight_Gsf_v1',
    'HLT_Ele105_CaloIdVT_GsfTrkIdT_v2',
    'HLT_Ele115_CaloIdVT_GsfTrkIdT_v1',
    'HLT_Mu16_eta2p1_CaloMET30_v2',
    'HLT_IsoMu16_eta2p1_CaloMET30_v2',
    'HLT_IsoMu16_eta2p1_CaloMET30_LooseIsoPFTau50_Trk30_eta2p1_v2',
    'HLT_IsoMu17_eta2p1_v2',
    'HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2',
    'HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1_v2',
    'HLT_IsoMu17_eta2p1_MediumIsoPFTau40_Trk1_eta2p1_Reg_v2',
    'HLT_DoubleIsoMu17_eta2p1_v2',
    'HLT_IsoMu24_eta2p1_LooseIsoPFTau20_v2',
    'HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v2',
    'HLT_IsoMu20_eta2p1_TriCentralPFJet30_v2',
    'HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v2',
    'HLT_IsoMu20_v2',
    'HLT_IsoMu20_eta2p1_v2',
    'HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v2',
    'HLT_IsoMu24_eta2p1_TriCentralPFJet30_v2',
    'HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v2',
    'HLT_IsoMu24_eta2p1_v2',
    'HLT_IsoMu27_v2',
    'HLT_IsoTkMu20_v2',
    'HLT_IsoTkMu20_eta2p1_v2',
    'HLT_IsoTkMu24_eta2p1_v2',
    'HLT_IsoTkMu27_v2',
    'HLT_JetE30_NoBPTX3BX_NoHalo_v2',
    'HLT_JetE30_NoBPTX_v2',
    'HLT_JetE50_NoBPTX3BX_NoHalo_v2',
    'HLT_JetE70_NoBPTX3BX_NoHalo_v2',
    'HLT_L1SingleMuOpen_v1',
    'HLT_L1SingleMuOpen_DT_v1',
    'HLT_L1SingleMu16_v1',
    'HLT_L2Mu10_v1',
    'HLT_L1Tech_DT_GlobalOR_v1',
    'HLT_L2DoubleMu23_NoVertex_v1',
    'HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10_v1',
    'HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10_v1',
    'HLT_L2Mu10_NoVertex_NoBPTX3BX_NoHalo_v1',
    'HLT_L2Mu10_NoVertex_NoBPTX_v1',
    'HLT_L2Mu35_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1',
    'HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX_NoHalo_v1',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_v2',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120_v2',
    'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80_v2',
    'HLT_Mu17_Mu8_v1',
    'HLT_Mu17_Mu8_DZ_v1',
    'HLT_Mu17_Mu8_SameSign_DZ_v1',
    'HLT_Mu20_Mu10_v1',
    'HLT_Mu20_Mu10_DZ_v1',
    'HLT_Mu20_Mu10_SameSign_DZ_v1',
    'HLT_Mu17_TkMu8_DZ_v2',
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v2',
    'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v2',
    'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v2',
    'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v2',
    'HLT_Mu25_TkMu0_dEta18_Onia_v2',
    'HLT_Mu27_TkMu8_v2',
    'HLT_Mu30_TkMu11_v2',
    'HLT_Mu40_TkMu11_v2',
    'HLT_Mu20_v1',
    'HLT_TkMu20_v2',
    'HLT_Mu24_eta2p1_v1',
    'HLT_TkMu24_eta2p1_v2',
    'HLT_Mu27_v1',
    'HLT_TkMu27_v2',
    'HLT_Mu50_v1',
    'HLT_Mu55_v1',
    'HLT_Mu45_eta2p1_v1',
    'HLT_Mu50_eta2p1_v1',
    'HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL_v2',
    'HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL_v2',
    'HLT_Mu28NoFiltersNoVtxDisplaced_Photon28_CaloIdL_v2',
    'HLT_Mu33NoFiltersNoVtxDisplaced_Photon33_CaloIdL_v2',
    'HLT_DoubleMu18NoFiltersNoVtx_v1',
    'HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL_v2',
    'HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Tight_v2',
    'HLT_Mu33NoFiltersNoVtxDisplaced_DisplacedJet50_Loose_v2',
    'HLT_Mu28NoFiltersNoVtx_DisplacedJet40_Loose_v2',
    'HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Tight_v2',
    'HLT_Mu38NoFiltersNoVtxDisplaced_DisplacedJet60_Loose_v2',
    'HLT_Mu38NoFiltersNoVtx_DisplacedJet60_Loose_v2',
    'HLT_Mu28NoFiltersNoVtx_CentralCaloJet40_v2',
    'HLT_PFHT350_PFMET100_NoiseCleaned_v1',
    'HLT_PFHT550_4Jet_v2',
    'HLT_PFHT650_4Jet_v2',
    'HLT_PFHT750_4JetPt50_v1',
    'HLT_PFHT600_v2',
    'HLT_PFHT650_v2',
    'HLT_PFHT800_v1',
    'HLT_PFJet40_v2',
    'HLT_PFJet60_v2',
    'HLT_PFJet80_v2',
    'HLT_PFJet140_v2',
    'HLT_PFJet200_v2',
    'HLT_PFJet260_v2',
    'HLT_PFJet320_v2',
    'HLT_PFJet400_v2',
    'HLT_PFJet450_v2',
    'HLT_PFJet500_v2',
    'HLT_DiPFJetAve40_v1',
    'HLT_DiPFJetAve60_v1',
    'HLT_DiPFJetAve80_v1',
    'HLT_DiPFJetAve140_v1',
    'HLT_DiPFJetAve200_v1',
    'HLT_DiPFJetAve260_v1',
    'HLT_DiPFJetAve320_v1',
    'HLT_DiPFJetAve400_v1',
    'HLT_DiPFJetAve500_v1',
    'HLT_DiPFJetAve60_HFJEC_v2',
    'HLT_DiPFJetAve80_HFJEC_v2',
    'HLT_DiPFJetAve100_HFJEC_v2',
    'HLT_DiPFJetAve160_HFJEC_v2',
    'HLT_DiPFJetAve220_HFJEC_v2',
    'HLT_DiPFJetAve300_HFJEC_v2',
    'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140_v2',
    'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu80_v2',
    'HLT_DiCentralPFJet55_PFMET110_NoiseCleaned_v1',
    'HLT_PFHT200_v1',
    'HLT_PFHT250_v1',
    'HLT_PFHT300_v1',
    'HLT_PFHT350_v2',
    'HLT_PFHT400_v1',
    'HLT_PFHT475_v1',
    'HLT_PFHT200_DiPFJetAve90_PFAlphaT0p57_v1',
    'HLT_PFHT200_DiPFJetAve90_PFAlphaT0p63_v1',
    'HLT_PFHT250_DiPFJetAve90_PFAlphaT0p55_v1',
    'HLT_PFHT250_DiPFJetAve90_PFAlphaT0p58_v1',
    'HLT_PFHT300_DiPFJetAve90_PFAlphaT0p53_v1',
    'HLT_PFHT300_DiPFJetAve90_PFAlphaT0p54_v1',
    'HLT_PFHT350_DiPFJetAve90_PFAlphaT0p52_v1',
    'HLT_PFHT350_DiPFJetAve90_PFAlphaT0p53_v1',
    'HLT_PFHT400_DiPFJetAve90_PFAlphaT0p51_v1',
    'HLT_PFHT400_DiPFJetAve90_PFAlphaT0p52_v1',
    'HLT_MET60_IsoTrk35_Loose_v1',
    'HLT_MET75_IsoTrk50_v2',
    'HLT_MET90_IsoTrk50_v2',
    'HLT_PFMET120_NoiseCleaned_BTagCSV0p72_v2',
    'HLT_PFMET120_NoiseCleaned_Mu5_v2',
    'HLT_PFMET170_NoiseCleaned_v2',
    'HLT_PFMET170_v1',
    'HLT_PFMET90_PFMHT90_IDTight_v1',
    'HLT_PFMET100_PFMHT100_IDTight_v1',
    'HLT_PFMET110_PFMHT110_IDTight_v1',
    'HLT_PFMET120_PFMHT120_IDTight_v1',
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV0p72_v1',
    'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_v1',
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq200_v2',
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq460_v2',
    'HLT_QuadPFJet_DoubleBTagCSV_VBF_Mqq240_v2',
    'HLT_QuadPFJet_SingleBTagCSV_VBF_Mqq500_v2',
    'HLT_QuadPFJet_VBF_v2',
    'HLT_L1_TripleJet_VBF_v1',
    'HLT_QuadJet45_TripleBTagCSV0p67_v2',
    'HLT_QuadJet45_DoubleBTagCSV0p67_v2',
    'HLT_DoubleJet90_Double30_TripleBTagCSV0p67_v2',
    'HLT_DoubleJet90_Double30_DoubleBTagCSV0p67_v2',
    'HLT_Photon135_PFMET100_NoiseCleaned_v2',
    'HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Photon250_NoHE_v2',
    'HLT_Photon300_NoHE_v2',
    'HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60_v2',
    'HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v2',
    'HLT_Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15_v1',
    'HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40_v2',
    'HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_VBF_v2',
    'HLT_Mu8_TrkIsoVVL_v2',
    'HLT_Mu17_TrkIsoVVL_v2',
    'HLT_Mu24_TrkIsoVVL_v2',
    'HLT_Mu34_TrkIsoVVL_v2',
    'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v2',
    'HLT_Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30_v2',
    'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v2',
    'HLT_Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30_v2',
    'HLT_BTagMu_DiJet20_Mu5_v2',
    'HLT_BTagMu_DiJet40_Mu5_v2',
    'HLT_BTagMu_DiJet70_Mu5_v2',
    'HLT_BTagMu_DiJet110_Mu5_v2',
    'HLT_BTagMu_Jet300_Mu5_v2',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v2',
    'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v2',
    'HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v2',
    'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL_v2',
    'HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v2',
    'HLT_Mu12_Photon25_CaloIdL_v2',
    'HLT_Mu12_Photon25_CaloIdL_L1ISO_v2',
    'HLT_Mu12_Photon25_CaloIdL_L1OR_v2',
    'HLT_Mu17_Photon30_CaloIdL_L1ISO_v2',
    'HLT_Mu17_Photon35_CaloIdL_L1ISO_v2',
    'HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v2',
    'HLT_TripleMu_12_10_5_v1',
    'HLT_Mu3er_PFHT140_PFMET125_NoiseCleaned_v2',
    'HLT_Mu6_PFHT200_PFMET80_NoiseCleaned_BTagCSV0p72_v1',
    'HLT_Mu6_PFHT200_PFMET100_NoiseCleaned_v1',
    'HLT_Mu14er_PFMET100_NoiseCleaned_v1',
    'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Ele17_CaloIdL_TrackIdL_IsoVL_v1',
    'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_v2',
    'HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV0p45_v2',
    'HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v2',
    'HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v2',
    'HLT_Photon22_v2',
    'HLT_Photon30_v2',
    'HLT_Photon36_v2',
    'HLT_Photon50_v2',
    'HLT_Photon75_v2',
    'HLT_Photon90_v2',
    'HLT_Photon120_v2',
    'HLT_Photon175_v2',
    'HLT_Photon165_HE10_v2',
    'HLT_Photon22_R9Id90_HE10_IsoM_v2',
    'HLT_Photon30_R9Id90_HE10_IsoM_v2',
    'HLT_Photon36_R9Id90_HE10_IsoM_v2',
    'HLT_Photon50_R9Id90_HE10_IsoM_v2',
    'HLT_Photon75_R9Id90_HE10_IsoM_v2',
    'HLT_Photon90_R9Id90_HE10_IsoM_v2',
    'HLT_Photon120_R9Id90_HE10_IsoM_v2',
    'HLT_Photon165_R9Id90_HE10_IsoM_v2',
    'HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95_v1',
    'HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1',
    'HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v1',
    'HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55_v1',
    'HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v1',
    'HLT_Dimuon0_Jpsi_Muon_v1',
    'HLT_Dimuon0_Upsilon_Muon_v1',
    'HLT_QuadMuon0_Dimuon0_Jpsi_v1',
    'HLT_QuadMuon0_Dimuon0_Upsilon_v1',
    'HLT_Rsq0p25_v1',
    'HLT_Rsq0p30_v1',
    'HLT_RsqMR240_Rsq0p09_MR200_v1',
    'HLT_RsqMR240_Rsq0p09_MR200_4jet_v1',
    'HLT_RsqMR270_Rsq0p09_MR200_v1',
    'HLT_RsqMR270_Rsq0p09_MR200_4jet_v1',
    'HLT_HT750_DisplacedDijet80_Inclusive_v2',
    'HLT_HT650_DisplacedDijet80_Inclusive_v2',
    'HLT_HT350_DisplacedDijet80_Tight_DisplacedTrack_v2',
    'HLT_HT350_DisplacedDijet40_DisplacedTrack_v2',
    'HLT_HT350_DisplacedDijet80_DisplacedTrack_v2',
    'HLT_HT500_DisplacedDijet40_Inclusive_v2',
    'HLT_HT550_DisplacedDijet40_Inclusive_v2',
    'HLT_VBF_DisplacedJet40_DisplacedTrack_v2',
    'HLT_VBF_DisplacedJet40_TightID_DisplacedTrack_v2',
    'HLT_VBF_DisplacedJet40_Hadronic_v2',
    'HLT_VBF_DisplacedJet40_TightID_Hadronic_v2',
    'HLT_VBF_DisplacedJet40_VTightID_Hadronic_v2',
    'HLT_VBF_DisplacedJet40_VVTightID_Hadronic_v2',
    'HLT_VBF_DisplacedJet40_VTightID_DisplacedTrack_v2',
    'HLT_VBF_DisplacedJet40_VVTightID_DisplacedTrack_v2',
    'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v2',
    'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v2',
    'HLT_MonoCentralPFJet80_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v2',
    'HLT_MonoCentralPFJet80_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v2',
    'HLT_CaloMET200_NoiseCleaned_v2',
    'HLT_Ele27_eta2p1_WPLoose_Gsf_HT200_v1',
    'HLT_Photon90_CaloIdL_PFHT500_v2',
    'HLT_Photon90_CaloIdL_PFHT600_v1',
    'HLT_DoubleMu8_Mass8_PFHT300_v2',
    'HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT300_v2',
    'HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT300_v2',
    'HLT_Mu10_CentralPFJet30_BTagCSV0p54PF_v2',
    'HLT_Ele10_CaloIdM_TrackIdM_CentralPFJet30_BTagCSV0p54PF_v2',
    'HLT_Ele15_IsoVVVL_BTagCSV0p72_PFHT400_v2',
    'HLT_Ele15_IsoVVVL_PFHT350_PFMET70_v1',
    'HLT_Ele15_IsoVVVL_PFHT600_v2',
    'HLT_Ele15_PFHT300_v2',
    'HLT_Mu10_TrkIsoVVL_DiPFJet40_DEta3p5_MJJ750_HTT350_PFMETNoMu60_v2',
    'HLT_Mu15_IsoVVVL_BTagCSV0p72_PFHT400_v2',
    'HLT_Mu15_IsoVVVL_PFHT350_PFMET70_v1',
    'HLT_Mu15_IsoVVVL_PFHT600_v2',
    'HLT_Mu15_PFHT300_v2',
    'HLT_Dimuon16_Jpsi_v1',
    'HLT_Dimuon10_Jpsi_Barrel_v1',
    'HLT_Dimuon8_PsiPrime_Barrel_v1',
    'HLT_Dimuon8_Upsilon_Barrel_v1',
    'HLT_Dimuon0_Phi_Barrel_v1',
    'HLT_Mu16_TkMu0_dEta18_Onia_v2',
    'HLT_Mu16_TkMu0_dEta18_Phi_v2',
    'HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx_v2',
    'HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v2',
    'HLT_Mu8_v1',
    'HLT_Mu17_v1',
    'HLT_Mu24_v1',
    'HLT_Mu34_v1',
    'HLT_Ele8_CaloIdM_TrackIdM_PFJet30_v2',
    'HLT_Ele12_CaloIdM_TrackIdM_PFJet30_v2',
    'HLT_Ele18_CaloIdM_TrackIdM_PFJet30_v2',
    'HLT_Ele23_CaloIdM_TrackIdM_PFJet30_v2',
    'HLT_Ele33_CaloIdM_TrackIdM_PFJet30_v2',
    'HLT_PFHT450_SixJet40_PFBTagCSV0p72_v2',
    'HLT_PFHT400_SixJet30_BTagCSV0p55_2PFBTagCSV0p72_v2',
    'HLT_PFHT450_SixJet40_v2',
    'HLT_PFHT400_SixJet30_v2',
    'HLT_PixelTracks_Multiplicity60ForEndOfFill_v1',
    'HLT_PixelTracks_Multiplicity85ForEndOfFill_v1',
    'HLT_PixelTracks_Multiplicity110ForEndOfFill_v1',
    'HLT_PixelTracks_Multiplicity135ForEndOfFill_v1',
    'HLT_PixelTracks_Multiplicity160ForEndOfFill_v1',
    'HLT_ECALHT800_v1',
    'HLT_Physics_v1',
    'DST_Physics_v1',
    'HLT_Random_v1',
    'HLT_ZeroBias_v1',
    'HLT_AK4CaloJet30ForEndOfFill_v1',
    'HLT_AK4CaloJet40ForEndOfFill_v1',
    'HLT_AK4CaloJet50ForEndOfFill_v1',
    'HLT_AK4CaloJet80_v2',
    'HLT_AK4CaloJet100_v2',
    'HLT_AK4PFJet30ForEndOfFill_v1',
    'HLT_AK4PFJet50ForEndOfFill_v1',
    'HLT_AK4PFJet80_v2',
    'HLT_AK4PFJet100_v2',
    'HLT_HISinglePhoton10_v2',
    'HLT_HISinglePhoton15_v2',
    'HLT_HISinglePhoton20_v2',
    'HLT_HISinglePhoton40_v2',
    'HLT_HISinglePhoton60_v2',
    'HLT_HIL1DoubleMu0_v1',
    'HLT_HIL2Mu3_v2',
    'HLT_HIL2DoubleMu0_v2',
    'HLT_HIL3Mu3_v2',
    'HLT_FullTrack12ForEndOfFill_v1',
    'HLT_FullTrack50_v2',
    'HLT_DiSC30_18_EIso_AND_HE_Mass70_v1',
    'HLT_Activity_Ecal_SC7_v1',
    'HLT_EcalCalibration_v1',
    'HLT_HcalCalibration_v1',
    'HLT_GlobalRunHPDNoise_v1',
    'HLT_L1Tech_HBHEHO_totalOR_v1',
    'HLT_L1Tech_HCAL_HF_single_channel_v1',
    'HLT_HcalNZS_v1',
    'HLT_HcalPhiSym_v1',
    'HLT_HcalUTCA_v1',
    'HLT_Photon500_v1',
    'HLT_Photon600_v1',
    'HLT_Mu300_v1',
    'HLT_Mu350_v1',
    'HLT_MET250_v1',
    'HLT_MET300_v1',
    'HLT_PFMET300_NoiseCleaned_v1',
    'HLT_PFMET400_NoiseCleaned_v1',
    'HLT_HT2000_v1',
    'HLT_HT2500_v1',
    'HLT_IsoTrackHE_v1',
    'HLT_IsoTrackHB_v1',
    'HLT_ZeroBias_part0_v1',
    'HLT_ZeroBias_part1_v1',
    'HLT_ZeroBias_part2_v1',
    'HLT_ZeroBias_part3_v1',
    'HLT_ZeroBias_part4_v1',
    'HLT_ZeroBias_part5_v1',
    'HLT_ZeroBias_part6_v1',
    'HLT_ZeroBias_part7_v1',
    'HLT_L1Tech5_BPTX_PlusOnly_v1',
    'HLT_L1Tech6_BPTX_MinusOnly_v1',
    'HLT_L1Tech7_NoBPTX_v1',
    ]
    
    calobjets = [
    'hltBLifetimeL3FilterCSVsusy',
    'hltCSV0p72L3',
    'hltCSVL30p74',
    'hltTripleCSV0p67',
    'hltDoubleCSV0p67',
    'hltBLifetimeL3FilterCSVLoose0p45',
    'hltCSV0p55L3',
    ]
    
    calobjetsMC = [
    'hltBLifetimeL3FilterCSVsusy',
    'hltCSV0p7L3',
    'hltCSVL30p6',
    'hltTripleCSV0p5',
    'hltDoubleCSV0p5',
    'hltBLifetimeL3FilterCSVLoose0p41',
    'hltCSV0p5L3',
    ]
    
    pfbjets = [
    'hltCSVFilterSingleTop',
    'hltDoubleCSVPF0p58',
    'hltCSVPF0p78',
    'hltCSV0p54FilterSingleMu10',
    'hltCSV0p54FilterSingleEle10',
    'hltCSVFilterPF0p72',
    'hlt2CSVFilterPF0p72',
    ]
    
    pfbjetsMC = [
    'hltCSVFilterSingleTop',
    'hltDoubleCSVPF0p4',
    'hltCSVPF0p7',
    'hltCSV0p5FilterSingleMu10',
    'hltCSV0p5FilterSingleEle10',
    'hltCSVFilterPF0p7',
    'hlt2CSVFilterPF0p7',
    ]
    
    MC = False
    if len(filesInput)>0 and ('AODSIM' in filesInput[0]):
        MC = True
    print "MC=",MC
    
    btags, btagLabel = Handle("edm::AssociationVector<edm::RefToBaseProd<reco::Jet>,vector<float>,edm::RefToBase<reco::Jet>,unsigned int,edm::helper::AssociationIdenticalKeyReference>"), ("pfCombinedInclusiveSecondaryVertexV2BJetTags") #("pfCombinedSecondaryVertexBJetTags")
    triggerEvent, triggerEventLabel = Handle("trigger::TriggerEvent"), ("hltTriggerSummaryAOD::HLT")
    
    if MC:
        btagLabel = ("combinedInclusiveSecondaryVertexV2BJetTags")
        triggerNames = triggerNamesMC
        calobjets = calobjetsMC
        pfbjets = pfbjetsMC
    
    ncalobjets = len(calobjets)
    npfbjets = len(pfbjets)
    nTriggers = len(triggerNames)

    nVertices = array( 'i', [ 0 ] )
    tree.Branch( 'nVertices', nVertices, 'nVertices/I' )
    run = array( 'i', [ 0 ] )
    tree.Branch( 'run', run, 'run/I' )
    lumi = array( 'i', [ 0 ] )
    tree.Branch( 'lumi', lumi, 'lumi/I' )
    eventNumber = array( 'i', [ 0 ] )
    tree.Branch( 'event', eventNumber, 'event/I' )
    JSON = array( 'i', [ 0 ] )
    tree.Branch( 'JSON', JSON, 'JSON/I' )
    
    caloMet = array( 'f', [ 0 ] )
    tree.Branch( 'caloMet', caloMet, 'caloMet/F' )
    caloMet_phi = array( 'f', [ 0 ] )
    tree.Branch( 'caloMet_phi', caloMet_phi, 'caloMet_phi/F' )
    
    pfMet = array( 'f', [ 0 ] )
    tree.Branch( 'pfMet', pfMet, 'pfMet/F' )
    pfMet_phi = array( 'f', [ 0 ] )
    tree.Branch( 'pfMet_phi', pfMet_phi, 'pfMet_phi/F' )
    
    caloMht = array( 'f', [ 0 ] )
    tree.Branch( 'caloMht', caloMht, 'caloMht/F' )
    caloMht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'caloMht_phi', caloMht_phi, 'caloMht_phi/F' )
    
    pfMht = array( 'f', [ 0 ] )
    tree.Branch( 'pfMht', pfMht, 'pfMht/F' )
    pfMht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'pfMht_phi', pfMht_phi, 'pfMht_phi/F' )
    
    caloNoPuMht = array( 'f', [ 0 ] )
    tree.Branch( 'caloNoPuMht', caloNoPuMht, 'caloNoPuMht/F' )
    caloNoPuMht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'caloNoPuMht_phi', caloNoPuMht_phi, 'caloNoPuMht_phi/F' )
    
    l1Met = array( 'f', [ 0 ] )
    tree.Branch( 'l1Met', l1Met, 'l1Met/F' )
    l1Met_phi = array( 'f', [ 0 ] )
    tree.Branch( 'l1Met_phi', l1Met_phi, 'l1Met_phi/F' )
    l1Met_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'l1Met_sumet', l1Met_sumet, 'l1Met_sumet/F' )
    
    l1Mht = array( 'f', [ 0 ] )
    tree.Branch( 'l1Mht', l1Mht, 'l1Mht/F' )
    l1Mht_phi = array( 'f', [ 0 ] )
    tree.Branch( 'l1Mht_phi', l1Mht_phi, 'l1Mht_phi/F' )
    l1Mht_sumet = array( 'f', [ 0 ] )
    tree.Branch( 'l1Mht_sumet', l1Mht_sumet, 'l1Mht_sumet/F' )
    
    l1Jet_num = array( 'i', [ 0 ] )
    tree.Branch( 'l1Jet_num', l1Jet_num, 'l1Jet_num/I' )
    l1Jet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_pt', l1Jet_pt, 'l1Jet_pt[l1Jet_num]/F' )
    l1Jet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_eta', l1Jet_eta, 'l1Jet_eta[l1Jet_num]/F' )
    l1Jet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_phi', l1Jet_phi, 'l1Jet_phi[l1Jet_num]/F' )
    l1Jet_mass = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_mass', l1Jet_mass, 'l1Jet_mass[l1Jet_num]/F' )
    l1Jet_tau = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_tau', l1Jet_tau, 'l1Jet_tau[l1Jet_num]/I' )
    l1Jet_offmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'l1Jet_offmatch', l1Jet_offmatch, 'l1Jet_offmatch[l1Jet_num]/I' )
    
    l1EG_num = array( 'i', [ 0 ] )
    tree.Branch( 'l1EG_num', l1EG_num, 'l1EG_num/I' )
    l1EG_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1EG_pt', l1EG_pt, 'l1EG_pt[l1EG_num]/F' )
    l1EG_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1EG_eta', l1EG_eta, 'l1EG_eta[l1EG_num]/F' )
    l1EG_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1EG_phi', l1EG_phi, 'l1EG_phi[l1EG_num]/F' )
    l1EG_iso = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'l1EG_iso', l1EG_iso, 'l1EG_iso[l1EG_num]/I' )
    l1EG_offmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'l1EG_offmatch', l1EG_offmatch, 'l1EG_offmatch[l1EG_num]/I' )
    
    l1Muon_num = array( 'i', [ 0 ] )
    tree.Branch( 'l1Muon_num', l1Muon_num, 'l1Muon_num/I' )
    l1Muon_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Muon_pt', l1Muon_pt, 'l1Muon_pt[l1Muon_num]/F' )
    l1Muon_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Muon_eta', l1Muon_eta, 'l1Muon_eta[l1Muon_num]/F' )
    l1Muon_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'l1Muon_phi', l1Muon_phi, 'l1Muon_phi[l1Muon_num]/F' )
    l1Muon_offmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'l1Muon_offmatch', l1Muon_offmatch, 'l1Muon_offmatch[l1Muon_num]/I' )
    
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
    
    caloJet_num = array( 'i', [ 0 ] )
    tree.Branch( 'caloJet_num', caloJet_num, 'caloJet_num/I' )
    caloJet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'caloJet_pt', caloJet_pt, 'caloJet_pt[caloJet_num]/F' )
    caloJet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'caloJet_eta', caloJet_eta, 'caloJet_eta[caloJet_num]/F' )
    caloJet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'caloJet_phi', caloJet_phi, 'caloJet_phi[caloJet_num]/F' )
    caloJet_mass = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'caloJet_mass', caloJet_mass, 'caloJet_mass[caloJet_num]/F' )
    caloJet_offmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'caloJet_offmatch', caloJet_offmatch, 'caloJet_offmatch[caloJet_num]/I' )
    caloJet_btagged ={}
    for calobjet in calobjets:
        caloJet_btagged[calobjet] = array( 'f', maxJets*[ 0 ] )
        tree.Branch( 'caloJet_'+calobjet, caloJet_btagged[calobjet], 'caloJet_'+calobjet+'[caloJet_num]/F' )
    
    pfJet_num = array( 'i', [ 0 ] )
    tree.Branch( 'pfJet_num', pfJet_num, 'pfJet_num/I' )
    pfJet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'pfJet_pt', pfJet_pt, 'pfJet_pt[pfJet_num]/F' )
    pfJet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'pfJet_eta', pfJet_eta, 'pfJet_eta[pfJet_num]/F' )
    pfJet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'pfJet_phi', pfJet_phi, 'pfJet_phi[pfJet_num]/F' )
    pfJet_mass = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'pfJet_mass', pfJet_mass, 'pfJet_mass[pfJet_num]/F' )
    pfJet_offmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'pfJet_offmatch', pfJet_offmatch, 'pfJet_offmatch[pfJet_num]/I' )
    pfJet_btagged ={}
    for pfbjet in pfbjets:
        pfJet_btagged[pfbjet] = array( 'f', maxJets*[ 0 ] )
        tree.Branch( 'pfJet_'+pfbjet, pfJet_btagged[pfbjet], 'pfJet_'+pfbjet+'[pfJet_num]/F' )
    
    offJet_num = array( 'i', [ 0 ] )
    tree.Branch( 'offJet_num', offJet_num, 'offJet_num/I' )
    offJet_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_pt', offJet_pt, 'offJet_pt[offJet_num]/F' )
    offJet_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_eta', offJet_eta, 'offJet_eta[offJet_num]/F' )
    offJet_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_phi', offJet_phi, 'offJet_phi[offJet_num]/F' )
    offJet_mass = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_mass', offJet_mass, 'offJet_mass[offJet_num]/F' )
    offJet_csv = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offJet_csv', offJet_csv, 'offJet_csv[offJet_num]/F' )
    offJet_pfmatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'offJet_pfmatch', offJet_pfmatch, 'offJet_pfmatch[offJet_num]/I' )
    offJet_calomatch = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'offJet_calomatch', offJet_calomatch, 'offJet_calomatch[offJet_num]/I' )
    offJet_l1match = array( 'i', maxJets*[ 0 ] )
    tree.Branch( 'offJet_l1match', offJet_l1match, 'offJet_l1match[offJet_num]/I' )

    
    offElectron_num = array( 'i', [ 0 ] )
    tree.Branch( 'offElectron_num', offElectron_num, 'offElectron_num/I' )
    offElectron_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_pt', offElectron_pt, 'offElectron_pt[offElectron_num]/F' )
    offElectron_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_eta', offElectron_eta, 'offElectron_eta[offElectron_num]/F' )
    offElectron_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_phi', offElectron_phi, 'offElectron_phi[offElectron_num]/F' )
    offElectron_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offElectron_iso', offElectron_iso, 'offElectron_iso[offElectron_num]/F' )
    
    offMuon_num = array( 'i', [ 0 ] )
    tree.Branch( 'offMuon_num', offMuon_num, 'offMuon_num/I' )
    offMuon_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_pt', offMuon_pt, 'offMuon_pt[offMuon_num]/F' )
    offMuon_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_eta', offMuon_eta, 'offMuon_eta[offMuon_num]/F' )
    offMuon_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_phi', offMuon_phi, 'offMuon_phi[offMuon_num]/F' )
    offMuon_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offMuon_iso', offMuon_iso, 'offMuon_iso[offMuon_num]/F' )
    
    offPhoton_num = array( 'i', [ 0 ] )
    tree.Branch( 'offPhoton_num', offPhoton_num, 'offPhoton_num/I' )
    offPhoton_pt = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_pt', offPhoton_pt, 'offPhoton_pt[offPhoton_num]/F' )
    offPhoton_eta = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_eta', offPhoton_eta, 'offPhoton_eta[offPhoton_num]/F' )
    offPhoton_phi = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_phi', offPhoton_phi, 'offPhoton_phi[offPhoton_num]/F' )
    offPhoton_iso = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPhoton_iso', offPhoton_iso, 'offPhoton_iso[offPhoton_num]/F' )
    
    
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
    tree.Branch( 'offPUPPIJet_phi', offPUPPIJet_phi, 'offPUPPIJet_phi[offPUPPIJet_num]/F' )
    offPUPPIJet_mass = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_mass', offPUPPIJet_mass, 'offPUPPIJet_mass[offPUPPIJet_num]/F' )
    offPUPPIJet_csv = array( 'f', maxJets*[ 0 ] )
    tree.Branch( 'offPUPPIJet_csv', offPUPPIJet_csv, 'offPUPPIJet_csv[offPUPPIJet_num]/F' )
    
    triggerBits, triggerBitLabel = Handle("edm::TriggerResults"), ("TriggerResults::HLT")
    triggerObjects, triggerObjectLabel  = Handle("std::vector<pat::TriggerObjectStandAlone>"), "selectedPatTrigger"
    triggerPrescales, triggerPrescaleLabel  = Handle("pat::PackedTriggerPrescales"), "patTrigger"
    
    patJets, patJetLabel = Handle("vector<reco::PFJet>"), ("ak4PFJets") #AOD

    l1JetCentrals, l1JetCentralLabel = Handle("vector<l1extra::L1JetParticle>"), ("l1extraParticles:Central") #AOD
    l1JetForwards, l1JetForwardLabel = Handle("vector<l1extra::L1JetParticle>"), ("l1extraParticles:Forward") #AOD
    l1JetTaus, l1JetTauLabel = Handle("vector<l1extra::L1JetParticle>"), ("l1extraParticles:Tau") #AOD
    l1JetIsoTaus, l1JetIsoTauLabel = Handle("vector<l1extra::L1JetParticle>"), ("l1extraParticles:IsoTau") #AOD

    l1EGNonIsos, l1EGNonIsoLabel = Handle("vector<l1extra::L1EmParticle>"), ("l1extraParticles:NonIsolated") #AOD
    l1EGIsos, l1EGIsoLabel = Handle("vector<l1extra::L1EmParticle>"), ("l1extraParticles:Isolated") #AOD

    l1METs, l1METLabel = Handle("vector<l1extra::L1EtMissParticle>"), ("l1extraParticles:MET") #AOD
    l1MHTs, l1MHTLabel = Handle("vector<l1extra::L1EtMissParticle>"), ("l1extraParticles:MHT") #AOD

    l1Muons, l1MuonLabel = Handle("vector<l1extra::L1MuonParticle>"), ("l1extraParticles") #AOD

    patMets, patMetLabel = Handle("vector<reco::PFMET>"), ("pfMet") #AOD
    recoVertexs, recoVertexLabel = Handle("vector<reco::Vertex>"), ("offlinePrimaryVertices") #AOD
    patElectrons, patElectronLabel = Handle("vector<reco::GsfElectron>"), ("gedGsfElectrons") #AOD
    patMuons, patMuonLabel = Handle("vector<reco::Muon>"), ("muons") #AOD
    patPhotons, patPhotonLabel = Handle("vector<reco::Photon>"), ("photons") #AOD
    
    triggerVars = {}
    for trigger in triggerNames:
        triggerVars[trigger]=array( 'i', [ 0 ] )
        tree.Branch( trigger, triggerVars[trigger], trigger+'/O' )

    if len(filesInput)==0: return
    events = Events (filesInput)
    
    memOld = 0
    for iev,event in enumerate(events):
        if iev%1000==0:
            print "iev=",iev
            memNew = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
            print 'Memory usage: %s (MB)'% (memNew/1000)
            print 'Diff', (memNew-memOld)/1000
            memOld = memNew
        if iev>maxevents: break
        event.getByLabel(triggerBitLabel, triggerBits)
#        event.getByLabel(triggerObjectLabel, triggerObjects)
#        event.getByLabel(triggerPrescaleLabel, triggerPrescales)
        event.getByLabel(triggerEventLabel, triggerEvent) ## AOD
        event.getByLabel(patJetLabel, patJets)
        event.getByLabel(patMetLabel, patMets)
        event.getByLabel(recoVertexLabel, recoVertexs)
        event.getByLabel(patElectronLabel, patElectrons)
        event.getByLabel(patMuonLabel, patMuons)
        event.getByLabel(patPhotonLabel, patPhotons)
        
        ## AOD
        event.getByLabel(btagLabel, btags)
        
        nVertices[0] = recoVertexs.product().size()
        run[0] = event.eventAuxiliary().run()
        lumi[0] = event.eventAuxiliary().luminosityBlock()
        eventNumber[0] = event.eventAuxiliary().event()
        JSON[0] = goodEvent(event.eventAuxiliary().run(),event.eventAuxiliary().luminosityBlock())
        
        i=0
        offJet_num[0] = 0
        for jet in patJets.product():
            if jet.pt()<20: continue
            if i<maxJets:                
                offJet_pt[i] = jet.pt()
                offJet_eta[i] = jet.eta()
                offJet_phi[i] = jet.phi()
                offJet_mass[i] = jet.mass()
                ## AOD
                offlineCSV = -1.
                for j in range(0,btags.product().size()):
                    jetB = btags.product().key(j).get()
                    dR = deltaR(jetB.eta(),jetB.phi(),jet.eta(),jet.phi())
                    if dR<0.3:
                        offlineCSV = max(0.,btags.product().value(j))
                        break
                
                offJet_csv[i] = offlineCSV
                offJet_num[0] = i + 1
                i+=1
        
        i=0
        offMuon_num[0] = 0
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
        offElectron_num[0] = 0
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
        offPhoton_num[0] = 0
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
        
        calojetCollection = "hltAK4CaloJetsCorrectedIDPassed"
        calojetCollectionForBtag = "hltSelector8CentralJetsL1FastJet"
        trigObjColl = triggerEvent.product().getObjects()
        collectionKeys = getCollectionKeys(triggerEvent.product(),ROOT.edm.InputTag(calojetCollection,"","HLT"))
        collectionKeysForBtag = getCollectionKeys(triggerEvent.product(),ROOT.edm.InputTag(calojetCollectionForBtag,"","HLT"))
        i=0
        caloJet_num[0] = 0
        for key in collectionKeys:
            caloJet = trigObjColl[key]
            if caloJet.pt()<20: continue
            if i<maxJets:
                caloJet_pt[i] = caloJet.pt()
                caloJet_eta[i] = caloJet.eta()
                caloJet_phi[i] = caloJet.phi()
                caloJet_mass[i] = caloJet.mass()
                caloJet_offmatch[i] = matching(caloJet.eta(),caloJet.phi(),offJet_eta,offJet_phi,offJet_num[0])
                caloJet_num[0] = i+1
                for calobjet in calobjets:
                    caloJet_btagged[calobjet][i] = -1
                    filterIndex = triggerEvent.product().filterIndex(ROOT.edm.InputTag(calobjet,"","HLT"))
                    if filterIndex < triggerEvent.product().sizeFilters():
                        for key3 in collectionKeysForBtag:
                            jetForBtag = trigObjColl[key3];
                            dR = deltaR(jetForBtag.eta(),jetForBtag.phi(),caloJet.eta(),caloJet.phi())
                            if dR<0.3:
                                caloJet_btagged[calobjet][i] = 0
                                break
                        filterKeys = triggerEvent.product().filterKeys(filterIndex)
                        for key2 in filterKeys:
                            bjet = trigObjColl[key2];
                            dR = deltaR(bjet.eta(),bjet.phi(),caloJet.eta(),caloJet.phi())
                            if dR<0.3:
                                caloJet_btagged[calobjet][i] = 1
                                break
            i+=1
        pfjetCollection = "hltAK4PFJetsCorrected"
        pfjetCollectionForBtag = "hltPFJetForBtag"
        trigObjColl = triggerEvent.product().getObjects()
        collectionKeys = getCollectionKeys(triggerEvent.product(),ROOT.edm.InputTag(pfjetCollection,"","HLT"))
        collectionKeysForBtag = getCollectionKeys(triggerEvent.product(),ROOT.edm.InputTag(pfjetCollectionForBtag,"","HLT"))
        i=0
        pfJet_num[0] = 0
        for key in collectionKeys:
            pfJet = trigObjColl[key]
            if pfJet.pt()<20: continue
            if i<maxJets:
                pfJet_pt[i] = pfJet.pt()
                pfJet_eta[i] = pfJet.eta()
                pfJet_phi[i] = pfJet.phi()
                pfJet_mass[i] = pfJet.mass()
                pfJet_offmatch[i] = matching(pfJet.eta(),pfJet.phi(),offJet_eta,offJet_phi,offJet_num[0])
                pfJet_num[0] = i+1
                for pfbjet in pfbjets:
                    pfJet_btagged[pfbjet][i] = -1
                    filterIndex = triggerEvent.product().filterIndex(ROOT.edm.InputTag(pfbjet,"","HLT"))
                    if filterIndex < triggerEvent.product().sizeFilters():
                        for key3 in collectionKeysForBtag:
                            jetForBtag = trigObjColl[key3];
                            dR = deltaR(jetForBtag.eta(),jetForBtag.phi(),pfJet.eta(),pfJet.phi())
                            if dR<0.3:
                                pfJet_btagged[pfbjet][i] = 0
                                break
                        filterKeys = triggerEvent.product().filterKeys(filterIndex)
                        for key2 in filterKeys:
                            bjet = trigObjColl[key2];
                            dR = deltaR(bjet.eta(),bjet.phi(),pfJet.eta(),pfJet.phi())
                            if dR<0.3:
                                pfJet_btagged[pfbjet][i] = 1
                                break
            i+=1
        
        for i in range(offJet_num[0]):
            offJet_pfmatch[i] = matching(offJet_eta[i],offJet_phi[i],pfJet_eta,pfJet_phi,pfJet_num[0])
            offJet_calomatch[i] = matching(offJet_eta[i],offJet_phi[i],caloJet_eta,caloJet_phi,caloJet_num[0])
        
        caloMETCollection = "hltMet"
        caloMet[0],caloMet_phi[0] = getMET(triggerEvent.product(),ROOT.edm.InputTag(caloMETCollection,"","HLT"))
        
        caloMHTCollection = "hltHtMht"
        caloMht[0],caloMht_phi[0] = getMET(triggerEvent.product(),ROOT.edm.InputTag(caloMHTCollection,"","HLT"))

        caloNoPuMHTCollection = "hltMHTNoPU"
        caloNoPuMht[0],caloNoPuMht_phi[0] = getMET(triggerEvent.product(),ROOT.edm.InputTag(caloNoPuMHTCollection,"","HLT"))

        pfMETCollection = "hltPFMETProducer"
        pfMet[0],pfMet_phi[0] = getMET(triggerEvent.product(),ROOT.edm.InputTag(pfMETCollection,"","HLT"))
        
        pfMHTCollection = "hltPFMHTTightID"
        pfMht[0],pfMht_phi[0] = getMET(triggerEvent.product(),ROOT.edm.InputTag(pfMHTCollection,"","HLT"))

        event.getByLabel(l1JetCentralLabel, l1JetCentrals)
        event.getByLabel(l1JetForwardLabel, l1JetForwards)
        event.getByLabel(l1JetTauLabel, l1JetTaus)
        if not MC:
            event.getByLabel(l1JetIsoTauLabel, l1JetIsoTaus)
            l1JetIsoTaus_=l1JetIsoTaus.product()
        else:
            l1JetIsoTaus_=[]
        i=0
        l1Jet_num[0] = 0
        for l1Jet in itertools.chain(l1JetCentrals.product(),l1JetForwards.product(),l1JetTaus.product(),l1JetIsoTaus_):
            l1Jet_pt[i] = l1Jet.pt()
            l1Jet_eta[i] = l1Jet.eta()
            l1Jet_phi[i] = l1Jet.phi()
            l1Jet_mass[i] = l1Jet.mass()
            l1Jet_offmatch[i] = matching(l1Jet.eta(),l1Jet.phi(),offJet_eta,offJet_phi,offJet_num[0])
            l1Jet_tau[i] = 0
            if l1Jet in l1JetTaus.product():
                l1Jet_tau[i] = 1
            if l1Jet in l1JetIsoTaus_:
                l1Jet_tau[i] = 2
            l1Jet_num[0] = i+1
            i+=1

        event.getByLabel(l1MuonLabel, l1Muons)
        i=0
        l1Muon_num[0] = 0
        for l1Muon in itertools.chain(l1Muons.product()):
            l1Muon_pt[i] = l1Muon.pt()
            l1Muon_eta[i] = l1Muon.eta()
            l1Muon_phi[i] = l1Muon.phi()
            l1Muon_offmatch[i] = matching(l1Muon.eta(),l1Muon.phi(),offMuon_eta,offMuon_phi,offMuon_num[0])
            l1Muon_num[0] = i+1
            i+=1

        event.getByLabel(l1EGIsoLabel, l1EGIsos)
        event.getByLabel(l1EGNonIsoLabel, l1EGNonIsos)
        i=0
        l1EG_num[0] = 0
        for l1EG in itertools.chain(l1EGIsos.product(),l1EGNonIsos.product()):
            l1EG_pt[i] = l1EG.pt()
            l1EG_eta[i] = l1EG.eta()
            l1EG_phi[i] = l1EG.phi()
            l1EG_offmatch[i] = matching(l1EG.eta(),l1EG.phi(),offElectron_eta,offElectron_phi,offElectron_num[0]) #ignoring offline photons
            l1EG_iso[i] = 0
            if l1EG in l1EGIsos.product():
                l1EG_iso[i] = 1
            l1EG_num[0] = i+1
            i+=1

        event.getByLabel(l1METLabel, l1METs)
        l1Met[0] = l1METs.product().begin().pt()
        l1Met_phi[0] = l1METs.product().begin().phi()
        l1Met_sumet[0] = l1METs.product().begin().etTotal()

        event.getByLabel(l1MHTLabel, l1MHTs)
        l1Mht[0] = l1MHTs.product().begin().pt()
        l1Mht_phi[0] = l1MHTs.product().begin().phi()
        l1Mht_sumet[0] = l1MHTs.product().begin().etTotal()

        names = event.object().triggerNames(triggerBits.product())
        for i,triggerName in enumerate(triggerNames):
            index = names.triggerIndex(triggerName)
            if checkTriggerIndex(triggerName,index,names.triggerNames()):
                triggerVars[triggerName][0] = triggerBits.product().accept(index)
            else:
                triggerVars[triggerName][0] = 0
        
        tree.Fill()
    
    
    f.Write()
    f.Close()

maxevents=200
fileOutput = 'tree.root'
filesInput=[
"BTagCSVRun2015B251883AOD.root",
#"TT_Spring15_AODSIM.root",
]
#launchNtupleFromAOD(fileOutput,filesInput,maxevents)
