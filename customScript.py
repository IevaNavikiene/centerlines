#!/usr/bin/env python
import sys
from vmtk import pypes
from vmtk import vmtkscripts

customscript = 'customScript'
class customScript(pypes.pypeScript):
	def __init__(self):
		pypes.pypeScript.__init__(self)
	def Execute(self): 
		pass
if __name__=='__main__':
	main = pypes.pypeMain()
	main.Arguments = sys.argv
	main.Execute()

