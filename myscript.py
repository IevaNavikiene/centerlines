#!/usr/bin/env python
from vmtk import pypes
from vmtk import vmtkscripts
import sys
import re
import os.path

arguments = sys.argv
name = arguments[1]
print('We are working with file named %s', name)
firstNamePart = re.split(r'\.(?!\d)', name)
print('All other files will have this (%s) part of name ', firstNamePart[0])
#segmentation vmtkimagereader -ifile small_vessel.tif --pipe vmtklevelsetsegmentation -ofile small_vessel.vti
if(not os.path.isfile(firstNamePart[0]+'.vti')): 
	segmentationArguments = 'vmtkimagereader -ifile '+name
	#+' --pipe vmtklevelsetsegmentation -ofile '+firstNamePart[0]+'.vti'#'vmtkmarchingcubes -ifile myimage.vti -l 800 --pipe vmtksurfaceviewer'
	myPype = pypes.PypeRun(segmentationArguments)
	myImage = myPype.GetScriptObject('vmtkimagereader','0').Image

	segmentator = vmtkscripts.vmtkLevelSetSegmentation()
	segmentator.Image = myImage
	segmentator.LevelSetsOutputFileName = firstNamePart[0]+'.vti'
	segmentator.Execute()
	myWriter = vmtkscripts.vmtkImageWriter()
	myWriter.Image = segmentator.Image
	myWriter.OutputFileName = firstNamePart[0]+'.vti'
	myWriter.Execute()

#convert to surface
if(not os.path.isfile(firstNamePart[0]+'.vtp')): 
	convertionArguments = 'vmtkmarchingcubes -ifile '+firstNamePart[0]+'.vti '
	myPype2 = pypes.PypeRun(convertionArguments)
	mySurface = myPype2.GetScriptObject('vmtkmarchingcubes','0').Surface
	
	#print(mySurface,'mySurface')
	mySmoother = vmtkscripts.vmtkSurfaceSmoothing()
	mySmoother.Surface = mySurface
	mySmoother.PassBand = 0.1
	mySmoother.NumberOfIterations = 30
	mySmoother.Execute()
	
	myWriter2 = vmtkscripts.vmtkSurfaceWriter()
	myWriter2.Surface = mySmoother.Surface
	myWriter2.OutputFileName = firstNamePart[0]+'.vtp'
	myWriter2.Execute()
print('pries centerlines')
#Extract network vmtknetworkextraction -ifile small_vessel.vtp -advancementratio 1.01 -ofile net_small_vessel.vtp
if(not os.path.isfile('net_'+firstNamePart[0]+'.vtp')): 
	print('priessssss centerlines')
	networkExtraction = vmtkscripts.vmtkNetworkExtraction()
	networkExtraction.Surface = mySmoother.Surface
	networkExtraction.AdvancementRatio = 1.01
	networkExtraction.Execute()
	'''
	centerlinesArguments = 'vmtknetworkextraction -ifile '+firstNamePart[0]+'.vtp -advancementratio 1.01 '
	myPype3 = pypes.PypeRun(centerlinesArguments)
	surface = myPype3.GetScriptObject('vmtknetworkextraction','0').Surface
	'''
	myWriter3 = vmtkscripts.vmtkSurfaceWriter()
	myWriter3.Surface = networkExtraction.Network
	myWriter3.OutputFileName = 'net_'+firstNamePart[0]+'.vtp'
	myWriter3.Execute()

	myWriter4 = vmtkscripts.vmtkSurfaceWriter()
	myWriter4.Surface = networkExtraction.GraphLayout
	myWriter4.OutputFileName = 'graph_'+firstNamePart[0]+'.vtp'
	myWriter4.Execute()
#Visualize vmtkrenderer --pipe vmtksurfaceviewer -ifile net_small_vessel.vtp  --pipe vmtksurfaceviewer -ifile small_vessel.vtp -opacity 0.3
visualizeArguments = 'vmtkrenderer --pipe vmtksurfaceviewer -ifile net_'+firstNamePart[0]+'.vtp  --pipe vmtksurfaceviewer -ifile '+firstNamePart[0]+'.vtp -opacity 0.3'
myPype4 = pypes.PypeRun(visualizeArguments)

# Filtrate "stars"
print('po vissssssssssssko')
'''
vmtkImageVesselEnhancement
'''
