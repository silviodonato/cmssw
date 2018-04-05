import os
from os import walk

server = "eoscms.cern.ch"

def findFileRecursive(server,folder,files):
    command = "xrdfs %s ls -l -u  %s"%(server,folder)
    #print(command)
    output = os.popen(command).read()
    lines = output.split("\n")
    for line in lines:
        tmp = ''
        while(tmp!=line):
            tmp = line
            line = line.replace("  "," ")
        words = line.split(" ")
        if(len(words)>0 and len(words[0])>0):
            #print words
            try:
                fullPath = words[4]
            except:
                print(words)
                fullPath = words[3]
            #print(fullPath)
            server =fullPath.split(folder)[0]
            path = folder+fullPath.split(folder)[1]
            if(words[0][0]=='d'):
                findFileRecursive(server, path,files)
            elif(".root" in path):
                files.append(path)
            elif(".tar.gz" in path):
                pass
            elif(".tmp" in path):
                pass
            else:
                print("WARNING: ", path)

folders = [
        "/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelIdealScenario_v1_HS-v2",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelRealScenario_v1_HS-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelScenarioA_v1_HS-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelScenarioB_v1_HS-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelScenarioC_v1_HS-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realisticv3_PixelScenarioD_v1_HS-v1",

        #"/store/relval/CMSSW_10_1_0_pre3/RelValH125GGgluonfusion_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValHiggs200ChargedTaus_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
##        "/store/relval/CMSSW_10_1_0_pre3/RelValNuGun/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
##        "/store/relval/CMSSW_10_1_0_pre3/RelValPREMIXUP18_PU25/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValQQH1352T_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
 ##       "/store/relval/CMSSW_10_1_0_pre3/RelValSMS-T1tttt_mGl-1500_mLSP-100_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTenE_0_200/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTenTau_15_500/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValTTbarLepton_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValZEE_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValZMM_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValZpTT_1500_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1",
        #"/store/relval/CMSSW_10_1_0_pre3/RelValZTT_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1"
    ]


for folder in folders:
    relval = folder.split("/")[4]
    GT = folder.split("/")[6]
    GT = "101X_"+GT.split("101X_")[1]
    GT = GT.split("_HS")[0]
    relval = relval + "_" + GT
    print(relval)
    
    os.popen("mkdir -p "+relval)
    os.popen("cp hlt.py "+relval)
    
    fileList = []
    findFileRecursive(server,folder,fileList)
    fList = str(fileList)
    fList = fList[1:-1]
    fList = fList.replace("/store/","root://%s//store/"%server)
    GT = '101X_upgrade2018_realisticv3_PixelRealScenario_v1'
    launch = """
from hlt import *

process.source.fileNames = cms.untracked.vstring(
        %s
    )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1000 )
)

process.GlobalTag.globaltag = cms.string('%s')
"""%(fList,GT)
    f = open(relval+"/launch.py",'w')
    f.write(launch)
    f.close()
    
    
    
    launchBsub ="""
#/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
export LOCAL_PATH=%s
export CMSSW_FOLDER=%s
cd $CMSSW_FOLDER
eval `scramv1 runtime -sh`
cd $LOCAL_PATH
echo $CMSSW_FOLDER
echo $LOCAL_PATH
echo $CMSSW_BASE
echo "cmsRun launch.py"
cmsRun launch.py
    """%(os.environ['PWD'] +"/"+relval,os.environ['CMSSW_BASE'])
    fb = open("%s/launch.sh"%relval,'w')
    os.popen('chmod +x %s/launch.sh'%relval)
    fb.write(launchBsub)
    fb.close()
    
#    print('cd %s && bsub -R "pool>30000" -q 8nm -J %s  launch.sh && cd -'%(relval,relval))
#    os.popen('cd %s && bsub -R "pool>30000" -q 8nm -J %s  launch.sh && cd -'%(relval,relval))
    print("")
    print(   'cd %s && qsub -l h_vmem=4g -q short.q -cwd  launch.sh && cd -'%(relval))
    os.popen('cd %s && qsub -l h_vmem=4g -q short.q -cwd  launch.sh && cd -'%(relval))
    print("")
    #qsub -q short.q -o RelValH125GGgluonfusion_13.log RelValH125GGgluonfusion_13/launch.sh 
#    os.popen('bsub -R "pool>30000" -q 8nm -J %s  < %s/launch.sh '%(relval,relval))

'''

from hlt import *

inputFiles = 'root://eoscms.cern.ch//store/relval/CMSSW_10_1_0_pre3/RelValTTbar_13/GEN-SIM-DIGI-RAW/PU25ns_101X_upgrade2018_realistic_v3-v1/10000/96DCB897-6029-E811-9805-0025905B8564.root'
process.source.fileNames = cms.untracked.vstring(inputFiles)
'''
