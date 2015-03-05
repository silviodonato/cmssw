'''

Creates L1ExtraNtuples (L1 Style) using a UCT->GT jump

Authors: L. Dodd, N. Woods, T. Perry, A. Levine,, S. Dasu, M. Cepeda, E. Friis (UW Madison)

'''

import FWCore.ParameterSet.Config as cms
import os

from FWCore.ParameterSet.VarParsing import VarParsing
process = cms.Process("ReRunningL1")

# Get command line options
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')

options.register(
    'isMC',
    1,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    'Set to 1 for simulated samples - updates GT, emulates HCAL TPGs.')

options.parseArguments()


process.source = cms.Source ("PoolSource",
    fileNames = cms.untracked.vstring(
      '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/02692B81-3D70-E311-810B-7845C4FC363E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/0E3D26A7-2D70-E311-BD27-00266CF9C1EC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/0E5D9340-F86F-E311-BDD8-7845C4FC3614.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/0E66F0C5-0770-E311-869B-848F69FD2889.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/127A7537-A870-E311-93E0-848F69FD2823.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/12F72745-4870-E311-A158-00266CF24EEC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/160D0960-C370-E311-9085-848F69FD45A4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/16248E9F-0170-E311-90D4-7845C4FC3983.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1667375A-9670-E311-B5F4-848F69FD455F.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/186D5D5C-F86F-E311-B122-7845C4FC3C98.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1896E457-0B70-E311-903C-7845C4FC35C9.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1AC4001C-C870-E311-9AAC-7845C4FC3785.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1CE39027-AC70-E311-B0F5-7845C4FC373A.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/1EA04802-CE70-E311-9CE4-001D09FDD7B9.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/222A3D89-FC6F-E311-82DA-00266CF9B8B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/281C4011-F26F-E311-826E-001D09FDD91E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/2A2729FF-9E70-E311-81A5-848F69FD2928.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/2EB77109-FF6F-E311-86CD-7845C4FC3683.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/320E197B-0370-E311-B2D0-848F69FD4409.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/32A480F3-0570-E311-A5B8-00A0D1EEAA00.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/348AE7FB-E56F-E311-9914-00266CF9C210.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/36A8C21F-FE6F-E311-9BB6-848F69FD28C8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/38FD4111-BD70-E311-A7F1-008CFA001B18.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/3A96ABAF-DE6F-E311-88AE-7845C4F92ECD.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/3C113A80-F86F-E311-AB9E-7845C4FC3785.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/3C549820-1170-E311-A4D5-008CFA008C94.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/3CED27F3-F76F-E311-B4E2-848F69FD2967.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/4A05FF50-0670-E311-8618-7845C4FC3683.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/4A9A1522-FD6F-E311-B80B-00266CF9B8B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/4E37172E-F96F-E311-B787-848F69FD2955.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/50E59D36-2870-E311-8F67-848F69FD45E3.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/521C7088-FD6F-E311-B723-00A0D1EE95FC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/522653AF-BB70-E311-AED3-7845C4FC373A.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/52B4CC6E-2270-E311-8A33-00A0D1EE29D0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/5630C36C-EA6F-E311-AE5B-7845C4FC3A28.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/5A8211DF-1970-E311-BD37-7845C4FC39F8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/6063F214-E06F-E311-BAE2-848F69FD453B.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/60BA7FF5-E86F-E311-BCE3-008CFA001444.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/627CCF5F-9770-E311-A0FA-7845C4FC378B.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/64692F88-EE6F-E311-8284-7845C4FC37C1.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/666F2D46-4271-E311-9E67-00A0D1EE8DFC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/6A30D433-C170-E311-8888-7845C4FC364D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/6AFB8CFC-B570-E311-BAF3-7845C4FC370D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/6C4871C0-A570-E311-82E2-008CFA0008C4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/744C5C55-B570-E311-8E03-00266CF9AEFC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/76BB0037-B970-E311-B982-00A0D1EEE3B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/7830732F-AF70-E311-953D-848F69FD2520.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/78960FE1-D36F-E311-A367-848F69FD29AF.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/789B2871-0270-E311-8E03-848F69FD457D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/7C422272-0270-E311-BA16-848F69FD454D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/7E7FD4D0-B170-E311-8000-848F69FD29D3.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/807F6B47-B670-E311-A747-7845C4F932D8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/84F97515-ED6F-E311-8205-7845C4F93215.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/866A5707-A670-E311-A595-7845C4F932B1.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/88429ABF-1270-E311-A099-848F69FD28EC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8E623383-3A70-E311-85AF-00266CFAE69C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/8ECBBA3B-8671-E311-A586-7845C4FC3683.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/905D90EF-AA70-E311-A21A-848F69FD29BB.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/90B36015-0070-E311-A986-008CFA001EE4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/90F0AFBD-CA70-E311-8363-7845C4FC3620.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/90FBFC15-9870-E311-A5BB-00266CF9B8B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9813196E-C270-E311-B6B4-848F69FD4EDD.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/983C7CA8-3870-E311-BB93-00A0D1EE952C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9A3A8E6F-0570-E311-9AF1-00A0D1EEF8EC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9AC24663-EF6F-E311-9E5E-00266CF9AD34.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9AC89DDD-A370-E311-860A-848F69FD45CB.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/9EBC46DA-0670-E311-893C-848F69FD45CB.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A06B7975-4B70-E311-A1B4-7845C4FC35E1.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A08B6E77-E46F-E311-BEE6-7845C4F914C5.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A23D37B9-B170-E311-929F-7845C4FC3608.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A242314C-2A70-E311-9E99-00266CFACC38.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A2B3C9BB-B070-E311-A675-848F69FD2910.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A2F3D09E-0E70-E311-8F72-00266CFAE6B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A87C510D-FA6F-E311-915D-001D09FDD7C5.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/A8BECED7-AB70-E311-A493-7845C4FC3B0C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/AE9B3011-B070-E311-B9AF-848F69FD2988.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/AEC9BEEF-F76F-E311-BD4D-180373FFCED8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/AEE33164-0070-E311-80CF-7845C4FC3B3F.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B444EA22-0470-E311-B2A8-848F69FD2967.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B44C9702-E36F-E311-9A3F-7845C4FC3611.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B4CABD0A-FF6F-E311-AC85-00A0D1EEE0C0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B4E77B5E-C070-E311-AFE2-848F69FD2967.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/B648CD2E-A570-E311-8A72-008CFA0008C4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/BA724DA6-A270-E311-8501-848F69FD4E8C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/BC68C116-B770-E311-B0CB-00A0D1EEE3B0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C4CF03D6-BE70-E311-B7A1-848F69FD43A0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C82CA970-0A70-E311-86EA-00A0D1EEF69C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/C89DB691-DE6F-E311-99D2-848F69FD453B.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/CC52D647-B870-E311-8C46-7845C4FC35F6.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D46F45AF-0870-E311-84B4-848F69FD0884.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D499DCFE-F76F-E311-B381-7845C4FC3785.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D88773C6-C470-E311-ADEE-7845C4F91621.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/D8B74DFE-8671-E311-9050-848F69FD4553.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/DA4FCD8B-0470-E311-8D27-00A0D1EEAA00.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/DCE7A9C6-1670-E311-A2BF-7845C4FC35CF.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/DE1D559A-F16F-E311-8C80-848F69FD2D6F.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/DE97352F-F86F-E311-96B7-00266CF9AEFC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E050596B-FB6F-E311-BFA9-00266CF25DF8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E0F0DEF9-3270-E311-B8ED-848F69FD2823.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E21EF345-4370-E311-B14B-848F69FD2940.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E4C394F5-B370-E311-9BD2-008CFA002EE0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E67C6F11-2170-E311-A3BB-7845C4FC375E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E6C39BEC-0470-E311-818D-7845C4FC3C4D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/E8866A5F-D070-E311-874B-848F69FD299D.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EC07AE79-A770-E311-85A7-848F69FD45E3.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EC4882C9-1570-E311-9377-00A0D1EE8A20.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EE9C345B-2A71-E311-97E9-7845C4FC3C11.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/EEA02549-F86F-E311-BBF2-00A0D1EE89E0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F0EEA18E-E16F-E311-A55D-7845C4FC35F6.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F43F2BE2-EB6F-E311-9471-7845C4F915E2.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F49103AF-AD70-E311-B342-00266CF9B878.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/F8EBDFC3-A970-E311-8DF1-00A0D1EE8EB4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00000/FAB7D317-0D70-E311-B6C7-848F69FD12B3.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/004B01A2-1170-E311-B5BE-008CFA008C94.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/0E0954A2-2470-E311-907A-00266CF9ADA0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/20B49E8D-E66F-E311-BC2F-7845C4FC39FB.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/342DC13B-BF70-E311-B7B8-7845C4FC3A2B.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/4095C4C6-F96F-E311-BE4E-7845C4FC35CF.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/503C64D1-E36F-E311-BE21-848F69FD4562.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/52AC78A6-4670-E311-9F61-008CFA00148C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/54D48FDF-0D70-E311-8AD1-00266CFAE7D0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/5A4003AA-A870-E311-93DE-848F69FD2988.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/5AB88F3B-DC6F-E311-8894-848F69FD2967.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/5E58E325-3B70-E311-9FA2-7845C4F92F87.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/604B15E1-CB70-E311-BD8F-180373FF94D6.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/66AF50A0-EF6F-E311-9143-00266CFAE30C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/66ED1AB3-F26F-E311-8359-00266CF9AD20.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/6AA74485-A670-E311-B66C-00266CF897A0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/70A06039-EC6F-E311-A1C0-008CFA00018C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/748BCC0C-A470-E311-984D-7845C4FC3A52.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/763A4795-D76F-E311-9D87-848F69FD28C2.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/76433961-B570-E311-95F3-848F69FD2943.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/76A40042-9870-E311-8833-008CFA002BB0.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/76B5AF21-B171-E311-86BA-848F69FD4DCC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/7C6BEB95-F16F-E311-AFE0-00266CF9B868.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/7E3CDB88-9A70-E311-9D27-008CFA001444.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/883246BA-9970-E311-A797-848F69FD471E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/8A736942-3870-E311-BDD6-00A0D1EEDFEC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/8E23F2AE-B170-E311-98C9-7845C4FC36E6.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/90783EF5-F56F-E311-BFD4-7845C4FC3B69.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/9E175E22-AB70-E311-9192-00266CF9BEB4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/A20E5B26-FB6F-E311-9603-00266CFAE264.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/A437A761-A270-E311-B930-7845C4FC3AC1.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/A6EAA4CA-E16F-E311-8A4D-008CFA0025E8.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/AAC91994-9C70-E311-9A33-008CFA002FF4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/AAD5DEDA-1670-E311-AFA9-00266CF24EEC.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/B60AD7CA-ED6F-E311-80A3-848F69FD4553.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/BE0642ED-1F70-E311-92E3-7845C4FC3C65.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/C6A9B745-DE6F-E311-90FF-7845C4FC3A91.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/CC8C0678-0070-E311-9077-848F69FD483E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/D2EF12EA-3B70-E311-93FE-00266CFAE69C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/DA325F84-F66F-E311-97F9-7845C4F92EAF.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/E4AFD48D-AE70-E311-86A4-848F69FD292E.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/E885DC46-A070-E311-8387-848F69FD29CA.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/F2EC0762-0770-E311-A00A-008CFA0020D4.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/F6FF366D-2E70-E311-BCCC-7845C4F9329C.root',
       '/store/mc/Fall13dr/ZH_HToBB_ZToNuNu_M-125_13TeV_powheg-herwigpp/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/20000/F871A5AC-E96F-E311-9157-7845C4F93215.root' 
),
#                             fileNames = cms.untracked.vstring(
#"/store/mc/Fall13dr/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00005/6AF2C1E2-DF7F-E311-B452-003048679162.root",
#                             )   
                             )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.TFileService = cms.Service("TFileService", fileName = cms.string("ntupla2L1ZnnHbb.root") ,      closeFileFast = cms.untracked.bool(True))
process.n = cms.EDAnalyzer("NtuplerL1Gen")
#process.path = cms.Path(process.n)



# Tested on Monte Carlo, for a test with data edit ahead
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'

# Load emulation and RECO sequences
if not options.isMC:
    process.load("L1Trigger.UCT2015.emulation_cfi")
    print "Running on data!"     
else:
    process.load("L1Trigger.UCT2015.emulationMC_cfi")

# Load sequences
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("L1Trigger.UCT2015.uctl1extraparticles_cfi")

process.p1 = cms.Path(
    process.emulationSequence *
    process.uct2015L1Extra *
    process.n
       #  *process.YourFavoritePlottingRoutine  --> This ends at l1extra production, anything after is up to the analyst 
)

# Make the framework shut up.
#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')

## Output definition
#process.output = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string('out.root'),
#    outputCommands = cms.untracked.vstring('drop *',
#          'keep *_*_*_ReRunningL1',
#          'keep *_l1extraParticles*_*_*') 
#)


# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:upgradePLS1')
    process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
    process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
    for pset in process.GlobalTag.toGet.value():
        pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#   Fix for multi-run processing:
    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
