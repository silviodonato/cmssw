import FWCore.ParameterSet.Config as cms

from DQM.EcalMonitorTasks.EcalMonitorTask_cfi import ecalMonitorTask
from DQM.EcalMonitorTasks.ecalGpuTask_cfi import ecalGpuTask


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

### replace offline inputtag with the online input tag
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

gpuVsCpuHLTsequence = cms.Sequence(
    hltGPUecalMonitorTask
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
