#!/usr/bin/env python
from vmtk import pypes
from vmtk import vmtkscripts
import sys
import re
import os.path
import argparse

def parse_args():
    """
    Parse input arguments
    """
    parser = argparse.ArgumentParser(description='Extract a CNN features')
    parser.add_argument('--smooth', dest='smooth_surface', help='Smooth surface',
                        default=0, type=int)
    parser.add_argument('--filename', dest='filename', help='Filename to extract centerlines', type=str)
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args()
    return args
    
arguments = parse_args()
print(arguments,'arguments')
name = arguments.filename

print('We are working with file named %s' % name)
firstNamePart = re.split(r'\.(?!\d)', name)
print('All other files will have this (%s) part of name ' % firstNamePart[0])

'''
#voi extraction    vmtkimagereader --ifile small_vessel.tif --pipe vmtkimagevoiselector -ofile -ofile voi_small_vessel.vti
if(not os.path.isfile('voi_'+firstNamePart[0]+'.vti')): 
	print('VOI extraction')
	segmentationArguments = 'vmtkimagereader -ifile '+name
	myPype0 = pypes.PypeRun(segmentationArguments)
	myImage = myPype0.GetScriptObject('vmtkimagereader','0').Image

	segmentator = vmtkscripts.vmtkImageVoiSelector()
	segmentator.Image = myImage
	segmentator.Execute()
	myWriter = vmtkscripts.vmtkImageWriter()
	myWriter.Image = segmentator.LevelSets
	myWriter.OutputFileName = 'voi_' + firstNamePart[0]+'.vti'
	myWriter.Execute()
'''
#segmentation vmtkimagereader -ifile small_vessel.tif --pipe vmtklevelsetsegmentation -ofile small_vessel.vti
if(not os.path.isfile(firstNamePart[0]+'.vti')): 
	print('Segmentation')
	segmentationArguments = 'vmtkimagereader -ifile '+ name
	#+' --pipe vmtklevelsetsegmentation -ofile '+firstNamePart[0]+'.vti'#'vmtkmarchingcubes -ifile myimage.vti -l 800 --pipe vmtksurfaceviewer'
	myPype = pypes.PypeRun(segmentationArguments)
	myImage = myPype.GetScriptObject('vmtkimagereader','0').Image

	segmentator = vmtkscripts.vmtkLevelSetSegmentation()
	segmentator.Image = myImage
	segmentator.Execute()
	myWriter = vmtkscripts.vmtkImageWriter()
	myWriter.Image = segmentator.LevelSets
	myWriter.OutputFileName = firstNamePart[0]+'.vti'
	myWriter.Execute()

#convert to surface vmtkmarchingcubes -ifile cproped_bin.vti -ofile cproped_bin.vtp
if(not os.path.isfile(firstNamePart[0]+'.vtp')): 
	print('Marching cubes')
	'''
	marchingCubes = vmtkscripts.vmtkMarchingCubes()
	marchingCubes.ImageInputFileName = firstNamePart[0]+'.vti'
	marchingCubes.Execute()
	'''
	#print(firstNamePart[0]+'.vti',"firstNamePart[0]+'.vti'")
	convertionArguments = 'vmtkmarchingcubes -ifile '+firstNamePart[0]+'.vti'
	myPype2 = pypes.PypeRun(convertionArguments)
	#print(dir(myPype2.GetScriptObject('vmtkmarchingcubes','0')))
	mySurface = myPype2.GetScriptObject('vmtkmarchingcubes','0').Surface
	if(arguments.smooth_surface == 1):
		print('smoothing...')
		#print(mySurface,'mySurface')
		mySmoother = vmtkscripts.vmtkSurfaceSmoothing()
		mySmoother.Surface = myPype2.GetScriptObject('vmtkmarchingcubes','0').Surface#marchingCubes.Surface
		mySmoother.PassBand = 0.1
		mySmoother.NumberOfIterations = 30
		mySmoother.Execute()
		
		myWriter2 = vmtkscripts.vmtkSurfaceWriter()
		myWriter2.Surface = mySmoother.Surface
		myWriter2.OutputFileName = firstNamePart[0]+'.vtp'
		myWriter2.Execute()
	else:
		myWriter2 = vmtkscripts.vmtkSurfaceWriter()
		myWriter2.Surface = myPype2.GetScriptObject('vmtkmarchingcubes','0').Surface
		myWriter2.OutputFileName = firstNamePart[0]+'.vtp'
		myWriter2.Execute()
# vmtksurfaceclipper -ifile foo_cl.vtp -ofile foo_cl2.vtp
if(not os.path.isfile('voi_'+firstNamePart[0]+'.vtp')):
    print('Extracting VOI')
    centerlinesArguments = 'vmtksurfaceclipper -ifile '+firstNamePart[0]+'.vtp -ofile voi_'+firstNamePart[0]+'.vtp'
    myPype4 = pypes.PypeRun(centerlinesArguments)

print('pries centerlines')
#Extract network vmtknetworkextraction -ifile small_vessel.vtp -advancementratio 1.01 -ofile net_small_vessel.vtp
if(not os.path.isfile('net_'+firstNamePart[0]+'.vtp')): 
	print('Centerlines')
	centerlinesArguments = 'vmtknetworkextraction -ifile voi_'+firstNamePart[0]+'.vtp -advancementratio 1.01 '
	myPype3 = pypes.PypeRun(centerlinesArguments)
	surface = myPype3.GetScriptObject('vmtknetworkextraction','0').Surface
	
	networkExtraction = vmtkscripts.vmtkNetworkExtraction()
	networkExtraction.Surface = surface#mySmoother.Surface
	networkExtraction.AdvancementRatio = 1.01
	networkExtraction.Execute()

	myWriter3 = vmtkscripts.vmtkSurfaceWriter()
	myWriter3.Surface = networkExtraction.Network
	myWriter3.OutputFileName = 'net_'+firstNamePart[0]+'.vtp'
	myWriter3.Execute()

	myWriter4 = vmtkscripts.vmtkSurfaceWriter()
	myWriter4.Surface = networkExtraction.GraphLayout
	myWriter4.OutputFileName = 'graph_'+firstNamePart[0]+'.vtp'
	myWriter4.Execute()
# Get length of centerline vmtkcenterlinegeometry -ifile net_Substack_edges_filtered_200_without_smoothing.vtp -ofile centerline-test.vtp
#with centerlined smoothing:vmtkcenterlinegeometry -ifile net_Substack_edges_filtered_200_without_smoothing.vtp -smoothing 1 -iterations 100 -factor 0.1 --pipe vmtksurfacewriter -i @.o -celldata 1 -ofile centerline-test.vtp
# this one is more clear becuse writes ressult to dat file: vmtkcenterlinegeometry -ifile net_Substack_edges_filtered_200_without_smoothing.vtp -smoothing 1 -iterations 100 -factor 0.1 --pipe vmtksurfacewriter -i @.o -celldata 1 -ofile christ3_ext_clgm.dat
#Visualize vmtkrenderer --pipe vmtksurfaceviewer -ifile net_small_vessel.vtp  --pipe vmtksurfaceviewer -ifile small_vessel.vtp -opacity 0.3
visualizeArguments = 'vmtkrenderer --pipe vmtksurfaceviewer -ifile net_'+firstNamePart[0]+'.vtp  --pipe vmtksurfaceviewer -ifile '+firstNamePart[0]+'.vtp -opacity 0.3'
myPype4 = pypes.PypeRun(visualizeArguments)

# Filtrate "stars"
print('po vissssssssssssko')
'''
vmtkImageVesselEnhancement
'''
