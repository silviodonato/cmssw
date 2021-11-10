### HLT customization functions for Run-3

You can apply the customization functions by adding this piece of code to your `hlt.py`
```
from customizeHLTforRun3 import *
process = TRK_newTracking(process)
#process = TAU_newL2sequence(process)
#process = BTV_addDeepJet(process)
#process = BTV_newCaloBTag(process)
#process = MUO_newTracking(process)
```

The list of customization functions available can be found at https://github.com/silviodonato/cms-tsg/blob/customizeHLTforRun3/customizeHLTforRun3.py

More info can be found in the google doc of the POG developments https://docs.google.com/spreadsheets/d/1nqd3qhFuM7TQgFRO_ZKNaGzCR0a21r4FLZQDC-JI-u0/edit#gid=0 

### Download and test

```
cmsrel CMSSW_12_1_0_pre5
cd CMSSW_12_1_0_pre5/src
cmsenv
git clone git@github.com:silviodonato/cms-tsg.git  hlt-run3
cd hlt-run3
edmConfigDump hlt.py > hlt_dump.py
```


### Credits

The customization functions have been downloaded from several repositories (see https://github.com/silviodonato/cms-tsg/blob/customizeHLTforRun3/Run3/downloadCustomizationFunctions.sh)

https://github.com/SWuchterl/RecoBTag-PerformanceMeasurements

https://github.com/mmasciov/cmssw/defaultRun3Tracking_forJIRA

https://github.com/annamasce/TauTriggerTools/triggerRnD_counter

https://github.com/khaosmos93/MuonHLTForRun3

