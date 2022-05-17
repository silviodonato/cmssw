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
    collectionTags = dict(
        EBCpuDigi = ("hltEcalDigisLegacy","ebDigis"),
        EBCpuRecHit = cms.untracked.InputTag("hltEcalRecHitWithTPs","EcalRecHitsEB"),
        EBCpuUncalibRecHit = cms.untracked.InputTag("hltEcalUncalibRecHitLegacy","EcalUncalibRecHitsEB"),
        EECpuDigi = cms.untracked.InputTag("hltEcalDigisLegacy","eeDigis"),
        EECpuRecHit = cms.untracked.InputTag("hltEcalRecHitWithTPs","EcalRecHitsEE"),
        EECpuUncalibRecHit = cms.untracked.InputTag("hltEcalUncalibRecHitLegacy","EcalUncalibRecHitsEE"),
        EBGpuDigi = cms.untracked.InputTag("hltEcalDigisFromGPU","ebDigis"),
        EBGpuRecHit = cms.untracked.InputTag("hltEcalRecHitWithoutTPs","EcalRecHitsEB"),
        EBGpuUncalibRecHit = cms.untracked.InputTag("hltEcalUncalibRecHitFromSoA","EcalUncalibRecHitsEB"),
        EEGpuDigi = cms.untracked.InputTag("hltEcalDigisFromGPU","eeDigis"),
        EEGpuRecHit = cms.untracked.InputTag("hltEcalRecHitWithoutTPs","EcalRecHitsEE"),
        EEGpuUncalibRecHit = cms.untracked.InputTag("hltEcalUncalibRecHitFromSoA","EcalUncalibRecHitsEE"),
    )
)

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
