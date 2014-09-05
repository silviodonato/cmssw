# set up variables
#def readConfig(fileName)
import FWCore.ParameterSet.Config as cms
from HLTriggerOffline.Btag.helper import *

class fileINI:
	def __init__(self, fileName):
		self.fileName=fileName

	def read(self):
		 Config.optionxform = str
		 Config.read(self.fileName)
		 self.processname=ConfigSectionMap("config")["processname"]
		 self.CMSSWVER=ConfigSectionMap("config")["cmsswver"]
		 self.jets=ConfigSectionMap("config")["hltjets"]
		 files=ConfigSectionMap("config")["files"]
		 self.maxEvents=ConfigSectionMap("config")["maxevents"]

		 files=files.splitlines()
		 self.files=filter(lambda x: len(x)>0,files)

		 self.btag_modules=cms.VInputTag()
		 self.btag_pathes=cms.vstring()
		 self.btag_modules_string=cms.vstring()
		 for path in Config.options("btag"):
		 	print path
		 	modules=Config.get("btag",path)
		 	modules=modules.splitlines()
		 	for module in modules:
		 		if(module!="" and path!=""):
				 	self.btag_modules.extend([cms.InputTag(module)])
				 	self.btag_modules_string.extend([module])
				 	self.btag_pathes.extend([path])

		 self.vertex_modules=cms.VInputTag()
		 self.vertex_pathes=cms.vstring()
		 for path in Config.options("vertex"):
		 	print path
		 	modules=Config.get("vertex",path)
		 	modules=modules.splitlines()
		 	for module in modules:
		 		if(module!="" and path!=""):
				 	self.vertex_modules.extend([cms.InputTag(module)])
				 	self.vertex_pathes.extend([path])


#>> 
#>>> 
#>>> 
#>>>  Config.read("my.ini")
#  File "<stdin>", line 1
#    Config.read("my.ini")
#    ^
#IndentationError: unexpected indent
#>>> 
#>>> 
#>>> 
#>>> 
#>>> 
#>>> Config = ConfigParser.ConfigParser()
#>>> import ConfigParser
#>>> Config = ConfigParser.ConfigParser()
#>>>  Config.read("my.ini")
#  File "<stdin>", line 1
#    Config.read("my.ini")
#    ^
#IndentationError: unexpected indent
#>>> 
#>>> Config.read("my.ini")
#['my.ini']
#>>> Config.sections()
#['config']
#>>> ConfigSectionMap
#Traceback (most recent call last):
#  File "<stdin>", line 1, in <module>
#NameError: name 'ConfigSectionMap' is not defined
#>>> Config.sections()
#['config']
#>>> Config.options("config")
#['maxevents', 'hltpathnames', 'processname', 'hltjets', 'cmsswver', 'files', 'taginfo']
#>>> Config.get("config","maxevents")


