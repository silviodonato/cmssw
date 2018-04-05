import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# load FWLite C++ libraries
#ROOT.gSystem.Load("libFWCoreFWLite.so");
#ROOT.gSystem.Load("libDataFormatsFWLite.so");
#ROOT.AutoLibraryLoader.enable()


folders = [
    "RelValH125GGgluonfusion_13",
    "RelValHiggs200ChargedTaus_13",
    "RelValNuGun",
    "RelValQQH1352T_13",
    "RelValSMS-T1tttt_mGl-1500_mLSP-100_13",
    "RelValTTbarLepton_13",
    "RelValTenE_0_200",
    "RelValZEE_13",
    "RelValZMM_13",
    "RelValZTT_13",
    "RelValQCD_FlatPt_15_3000HS_13",
    "RelValTTbar_13",
    "RelValZpTT_1500_13",
    "RelValTenTau_15_500",
    "RelValPREMIXUP18_PU25",
]

c1 = ROOT.TCanvas()
for folder in folders:
    print ("-"*30)
    print(folder)
    print ("-"*30)
    try:
        f = ROOT.TFile(folder+"/output.root")
        e = f.Get("Events")
    #    print(e.Print())
        e.Draw("abs(recoVertexs_hltPixelVertices__HLTX.obj[0].position_.fCoordinates.fZ - SimVertexs_g4SimHits__SIM.obj[0].theVertex.fCoordinates.fZ)<0.3 >> new ")
        print("New:"+str(f.Get("new").GetMean()))
        e.Draw("abs(recoVertexs_hltTrimmedPixelVertices__HLT.obj[0].position_.fCoordinates.fZ - SimVertexs_g4SimHits__SIM.obj[0].theVertex.fCoordinates.fZ)<0.3 >> old ")
        print("Old:"+str(f.Get("old").GetMean()))
    except:
        print("not ready")
