
from hlt import process,_customInfo
from HLTrigger.Configuration.customizeHLTforRun3 import *

process = TRK_newTracking(process)

process = MUO_updateTkMu(process)


### Drop for confdb ###
els = process.__dict__
for el in list(els):
#    if  (type(els[el]) == cms.OutputModule) or (type(els[el]) == cms.EndPath)   or (type(els[el]) == cms.Service) or (type(els[el]) == cms.PSet) or (type(els[el]) == cms.ESProducer)  or (type(els[el]) == cms.ESSource):
    if  (type(els[el]) == cms.OutputModule) or (type(els[el]) == cms.EndPath):
#    if (  (type(els[el]) != cms.Path) and (type(els[el]) != cms.Sequence) and (type(els[el]) != cms.Task) and (type(els[el]) != cms.SwitchProducer) and (type(els[el]) != cms.EDProducer) and (type(els[el]) != cms.EDFilter) and el!="source" ):
#        print("Deleting %s (%s)"%(el, type(els[el])))
        delattr(process, el)

