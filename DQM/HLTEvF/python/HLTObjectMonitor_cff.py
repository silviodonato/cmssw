import FWCore.ParameterSet.Config as cms

###### ECAL #####################
from DQM.EcalMonitorTasks.EcalMonitorTask_cfi import ecalMonitorTask
from DQM.EcalMonitorTasks.ecalGpuTask_cfi import ecalGpuTask

# renaming and enable only GPU-CPU comparison plot
hltGPUecalMonitorTask = ecalMonitorTask.clone(
    moduleName = "HLT Ecal Monitor Source GPU vs CPU",
    workers = ["GpuTask"],
    workerParameters = cms.untracked.PSet(
        GpuTask = ecalGpuTask.clone(
            params = dict(
                dict(
                    gpuOnlyPlots = False,
                    runGpuTask = True
                )
            ), 
        ),
    ),
)

# replace offline inputtag with the online input tag
for par in hltGPUecalMonitorTask.collectionTags.parameterNames_():
    par = getattr(hltGPUecalMonitorTask.collectionTags,par)
    
    par.setValue(par.value().replace("ecalMultiFitUncalibRecHit@cpu","hltEcalUncalibRecHitLegacy"))
    par.setValue(par.value().replace("ecalMultiFitUncalibRecHit@cuda","hltEcalUncalibRecHitFromSoA"))
    par.setValue(par.value().replace("ecalMultiFitUncalibRecHit","hltEcalUncalibRecHitFromSoA"))
    
    par.setValue(par.value().replace("ecalDigis@cpu","hltEcalDigisLegacy"))
    par.setValue(par.value().replace("ecalDigis@cuda","hltEcalDigisFromGPU"))
    par.setValue(par.value().replace("ecalDigis","hltEcalDigisLegacy"))
    
    par.setValue(par.value().replace("ecalRecHit@cpu","hltEcalRecHitWithTPs"))
    par.setValue(par.value().replace("ecalRecHit@cuda","hltEcalRecHitWithoutTPs"))
    par.setValue(par.value().replace("ecalRecHit","hltEcalRecHitWithoutTPs"))


###### HCAL #####################
from DQM.HcalTasks.hcalGPUComparisonTask_cfi import hcalGPUComparisonTask

# replace offline inputtag with the online input tag
hltGPUhcalMonitorTask = hcalGPUComparisonTask.clone(
    name = ('hltHcalGPUComparisonTask'),
    tagHBHE_ref = ("hltHbherecoLegacy"),
    tagHBHE_target = ("hltHbherecoFromGPU"),
)

###### Pixel #####################

from DQM.SiPixelPhase1Heterogeneous.siPixelPhase1CompareTrackSoA_cfi import *
from DQM.SiPixelPhase1Heterogeneous.siPixelPhase1CompareVertexSoA_cfi import *
from DQM.SiPixelPhase1Heterogeneous.siPixelPhase1CompareRecHitsSoA_cfi import *

hltGPUsiPixelPhase1CompareTrackSoA = siPixelPhase1CompareTrackSoA.clone(
    pixelTrackSrcCPU = ("hltPixelTracksCPU"), #ie. hltPixelTracksSoA@cpu
    pixelTrackSrcGPU = ("hltPixelTracksFromGPU"), #ie. hltPixelTracksSoA@cuda
    topFolderName = 'SiPixelHeterogeneous/HLTPixelTrackCompareGPUvsCPU',
)
hltGPUsiPixelPhase1CompareVertexSoA = siPixelPhase1CompareVertexSoA.clone(
    pixelVertexSrcCPU = ("hltPixelVerticesCPU"), #ie. hltPixelVerticesSoA@cpu
    pixelVertexSrcGPU = ("hltPixelVerticesFromGPU"), #ie. hltPixelVerticesSoA@cuda
    topFolderName = 'SiPixelHeterogeneous/HLTPixelTrackCompareGPUvsCPU',
)

hltGPUsiPixelPhase1CompareRecHitsSoA = siPixelPhase1CompareRecHitsSoA.clone(
    pixelHitsSrcCPU = ("hltSiPixelRecHitsLegacy"), #ie. hltSiPixelRecHits@cpu
    pixelHitsSrcGPU = ("hltSiPixelRecHitSoAFromGPU"), #ie. hltSiPixelRecHits@cuda
    topFolderName = 'SiPixelHeterogeneous/HLTPixelRecHitsCompareGPUvsCPU'
)



##from DQM.SiPixelPhase1Heterogeneous.siPixelPhase1CompareRecHitsSoA_cfi import *

#hltGPUsiPixelPhase1CompareTrackSoA = cms.EDProducer("SiPixelPhase1CompareTrackSoA",
#    deltaR2cut = cms.double(0.04),
#    mightGet = cms.optional.untracked.vstring,
#    minQuality = cms.string('loose'),
#    pixelTrackSrcCPU = ("pixelTracksSoA@cpu"),
#    pixelTrackSrcGPU = cms.InputTag("pixelTracksSoA@cuda"),
#    topFolderName = cms.string('SiPixelHeterogeneous/PixelTrackCompareGPUvsCPU'),
#    useQualityCut = cms.bool(True)
#)


#hltGPUsiPixelPhase1CompareVertexSoA = cms.EDProducer("SiPixelPhase1CompareVertexSoA",
#    beamSpotSrc = cms.InputTag("offlineBeamSpot"),
#    dzCut = cms.double(1),
#    mightGet = cms.optional.untracked.vstring,
#    pixelVertexSrcCPU = cms.InputTag("pixelVerticesSoA@cpu"),
#    pixelVertexSrcGPU = cms.InputTag("pixelVerticesSoA@cuda"),
#    topFolderName = cms.string('SiPixelHeterogeneous/PixelVertexCompareSoAGPUvsCPU')
#)


process.hltGPUsiPixelPhase1CompareRecHitsSoA = cms.EDProducer("SiPixelPhase1CompareRecHitsSoA",
    mightGet = cms.optional.untracked.vstring,
    minD2cut = cms.double(0.0001),
    pixelHitsSrcCPU = cms.InputTag("siPixelRecHitsPreSplittingSoA@cpu"),
    pixelHitsSrcGPU = cms.InputTag("siPixelRecHitsPreSplittingSoA@cuda"),
    topFolderName = cms.string('SiPixelHeterogeneous/PixelRecHitsCompareGPUvsCPU')
)


###### Sequence #####################

gpuVsCpuHLTsequence = cms.Sequence(
    hltGPUecalMonitorTask +
    hltGPUhcalMonitorTask +
    hltGPUsiPixelPhase1CompareRecHitsSoA
#    hltGPUsiPixelPhase1CompareTrackSoA +
#    hltGPUsiPixelPhase1CompareVertexSoA
)


hlt4vector = cms.Path(
#    lumiOnlineMonitorHLTsequence # lumi
#    * hltObjectMonitor
#    * hcalOnlineMonitoringSequence # HCAL monitoring
#    * pixelOnlineMonitorHLTsequence # pixel cluster monitoring
#    * sistripOnlineMonitorHLTsequence # strip cluster monitoring
#    * trackingMonitoringHLTsequence # tracking monitoring
#    * egmTrackingMonitorHLTsequence # EGM tracking monitoring
#    * vertexingMonitorHLTsequence # vertexing
#    * hltObjectsMonitor
    gpuVsCpuHLTsequence # GPU vs CPU sequence
)
