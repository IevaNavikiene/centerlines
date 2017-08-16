# import the necessary packages
import numpy as np
import argparse
import cv2
from PIL import Image
import time
from tifffile import imsave
import csv
import math
#import shapely.geometry as geometry
import sys 
import matplotlib.path as mplPath
'''
Function draw boundaries for ground truth and automatic generated centerlines
Color instructions:
Blue - ground truth found match with my centerlines
Red - ground truth not found in my centerlines
Green - my centerlines found in ground truth
Yellow - my centerlines not found in ground truth
Orange - stalled vessels
'''

# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required = True, help = "Path to the image")
ap.add_argument("-g", "--ground", required = True, help = "Path to the ground truth centerline csv document")
ap.add_argument("-c", "--centerline", required = True, help = "Path to the centerline csv document")
ap.add_argument("-d", "--onlyDraw", required = False, default=0, help = "If true then ground truth and centerlines will be only drawn in new tif image stack")
args = vars(ap.parse_args())

outputImageName = "compared.tif"
# load the image, clone it for output, and then convert it to grayscale
image = Image.open(args['image'])

frameCount = image.n_frames
width, height = image.size
print(width, height,"width, height")
outputImage = np.zeros((frameCount, height, width,3 ), 'uint16')

#open csv document and form array to draw centerlines
def centerlinesCoordinates(filename, offset, addStalledData = False):
    centerlinesCoordinates = []
    with open(filename, 'rb') as csvfile:
       spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
       for row in spamreader:
           rowArray = row[0].split(',')
           if(len(rowArray) < 4):
               continue
               
           #convert coordinates to float
           rowArray[1] = int(math.floor(float(rowArray[1])));
           rowArray[2] = int(math.floor(float(rowArray[2])));
           rowArray[3] = int(math.floor(float(rowArray[3])))-offset;#
           rowArray[0] = int((rowArray[0]))
           #print(rowArray[0],"rowArray[0]",len(centerlinesCoordinates),"len(centerlinesCoordinates)")
           #sys.exit(1)
           if(rowArray[3]>=0 and rowArray[3]<=frameCount):
               if rowArray[0] < len(centerlinesCoordinates):
                   centerlinesCoordinates[rowArray[0]][0].append([rowArray[1], rowArray[2]])
                   #if z coordinate is smaller than previous then change it (and the same for max z coordinate)
                   if(rowArray[3] < centerlinesCoordinates[rowArray[0]][1][0]):
                       centerlinesCoordinates[rowArray[0]][1][0] = rowArray[3]
                   elif(rowArray[3] > centerlinesCoordinates[rowArray[0]][1][1]):
                       centerlinesCoordinates[rowArray[0]][1][1] = rowArray[3]
               else:
                   stalled= '0'
                   if(addStalledData==True):
                       stalled = rowArray[4]
                   centerlinesCoordinates.append([[[rowArray[1], rowArray[2]]],[rowArray[3], rowArray[3]], stalled])
               
    return  np.array(centerlinesCoordinates)
    
def checkIfPointExistInContour(centerlineCoord, arrayShape, centerCoordinates, usedCenterlines, frameRange):
    '''
     Function finds the best percentage match for the contour
     If two vessels have the same percentage - will be chosen vessel with biggest points count
     centerCoordinates, usedCenterlines - used centerlines helps to determin which centerlines can't be used again (to not create a duplicate)
    '''
    reshapedContour = np.reshape(centerlineCoord, (arrayShape[0], arrayShape[1], arrayShape[3]))
    mplContour = mplPath.Path(reshapedContour[0])
    bestFitLineResult = 0.0
    maxLineFit = 0
    bestFitLineId = -1
    lineId = -1
    
    for centerline in centerCoordinates:
        lineId += 1
        lineFit = 0.0
        counter = 0
        if(lineId in usedCenterlines):
            continue
        if(((centerline[1][0] < frameRange[0]) and (centerline[1][1] < frameRange[0])) or \
            ((centerline[1][0] > frameRange[1]) and (centerline[1][1] > frameRange[1]))):
            continue
        for point in centerline[0]:
            pointsExist = mplContour.contains_point((point[0], point[1]))
            if(pointsExist != False):
                lineFit += 1.0
                #print(pointsExist,'pointsExist')
            counter += 1
        if(bestFitLineResult < lineFit/counter or (bestFitLineResult == lineFit/counter and lineFit>maxLineFit)):
            bestFitLineResult = lineFit/counter
            bestFitLineId = lineId
            maxLineFit = lineFit
    pointExist = 1 if (bestFitLineResult>0.2) else 0   
    #print(bestFitLineResult,"bestFitLineResult")     
    return (pointExist, bestFitLineResult, bestFitLineId)
    
def makeContoursList(frameCount, centerlinesCoord, size = 20, stalled= False):
    contoursList = []
    for i in range(frameCount):
        contoursList.append([])
        #print(centerlinesCoord)
        for centerlineCoord in centerlinesCoord:
            if(stalled == True and centerlineCoord[2] == '0'):
                continue
            if(centerlineCoord[1][0] <= i and centerlineCoord[1][1]>=i):
                fakeImage = np.zeros((height, width), dtype =np.uint8)
                convertedArray = np.array([np.array(xi, np.int32) for xi in centerlineCoord[0]])
                pts = convertedArray.reshape((-1,1,2))
                cv2.polylines(fakeImage,[pts],False,(255,0,0),size)
                _ret, contours, hierarchy = cv2.findContours(fakeImage,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE) 
                contoursList[i].append(contours)
    return contoursList
    
centerCoordinates = centerlinesCoordinates(args['centerline'], -29)
#groundTruthCoordinates = centerlinesCoordinates(args['centerline'], 0)
groundTruthCoordinates = centerlinesCoordinates(args['ground'], 0, True)
'''
Find countour of each vessel
'''
#boundariesList = np.zeros((frameCount, height, width), dtype =np.uint8)

uniqueContoursList=[]
uniqueContoursFrameRange=[]
if(args['onlyDraw'] == 1):
    contoursList = makeContoursList(frameCount, groundTruthCoordinates)
    '''
    Draw contours and save image
    '''
    for i in range(frameCount):
        try:
            image.seek(i)
            imageRGB = image.convert('RGB')
            outputImage[i,:,:] = np.array(imageRGB) 
            for contours in contoursList[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (255,0,0), 2)
        except EOFError:
            print("Error")
            # Not enough frames in img
            break

    print("Saving tif stack to %s" % outputImageName)
    imsave(outputImageName, outputImage)
else:
    #convert line points to contour coordinates
    for groundTruthCoord in groundTruthCoordinates:
        fakeImage = np.zeros((height, width), dtype =np.uint8)
        convertedArray = np.array([np.array(xi, np.int32) for xi in groundTruthCoord[0]])
        pts = convertedArray.reshape((-1,1,2))
        cv2.polylines(fakeImage,[pts],False,(255,0,0),20)
        _ret, contours, hierarchy = cv2.findContours(fakeImage,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE) 
        uniqueContoursList.append(contours)
        uniqueContoursFrameRange.append(groundTruthCoord[1])

    contourId = 0 
    allPointsFound = 0
    foundInGroundTruth = [] #exist in both lists (id from my list)
    notFoundInGroundTruth = [] #exist in my list but do not exist in ground truth
    notFoundVessels = []# exist in ground truth but do not exist in my list
    foundInGroundTruthGroundId = []#exist in both lists (id from ground truth list) 
    for centerlineCoord in uniqueContoursList:
        centerlineCoord = np.array(centerlineCoord)
        #print('aaaaa', centerlineCoord.shape)
        if(centerlineCoord != []):
            arrayShape = centerlineCoord.shape
            #TODO when I have more contours in one contour???
            if(len(arrayShape) < 3):
                #print('arraySh', centerlineCoord[0].shape)
                for i in range(len(arrayShape)):
                    arrayShape = np.array([centerlineCoord[i]]).shape
                    pointExist, bestFitLineResult, bestFitLineId = checkIfPointExistInContour(centerlineCoord[i], arrayShape, centerCoordinates, foundInGroundTruth, uniqueContoursFrameRange[contourId])
                    # bestFitLineId will be equal to -1 if it was not found vessel in contour. P.s. bestFitLineId is my centerlines list id
                    if(bestFitLineId != -1):
                        allPointsFound += pointExist
                        foundInGroundTruth.append(bestFitLineId)
                        foundInGroundTruthGroundId.append(contourId)
                    else:
                        notFoundVessels.append(contourId)
            else:
                pointExist, bestFitLineResult, bestFitLineId = checkIfPointExistInContour(centerlineCoord, arrayShape, centerCoordinates, foundInGroundTruth, uniqueContoursFrameRange[contourId])
                if(bestFitLineId != -1):
                    allPointsFound += pointExist
                    foundInGroundTruth.append(bestFitLineId)
                    foundInGroundTruthGroundId.append(contourId)
                else:
                    notFoundVessels.append(contourId)
            #print('arrayShape', arrayShape)
        contourId += 1
        
    '''
    Make a list if my vessel do not exist in ground truth boundary
    '''
    
    for i in range(len(centerCoordinates)):
        if(not i in foundInGroundTruth):
            notFoundInGroundTruth.append(i)
    print('Found vessels comparing to ground truth', allPointsFound, 'Counted ground truth vessels', contourId)
    print('Vessels which do not exist in ground truth', len(notFoundInGroundTruth), 'Counted my found vessels', len(centerCoordinates))

    '''
    Draw contours and save image
    '''
    #contours which existed in ground truth but not in my list
    contoursListRed = makeContoursList(frameCount, groundTruthCoordinates[notFoundVessels])
    contoursListBlue = makeContoursList(frameCount, groundTruthCoordinates[foundInGroundTruthGroundId])
    contoursListGreen = makeContoursList(frameCount, centerCoordinates[foundInGroundTruth],15)
    contoursListYellow = makeContoursList(frameCount, centerCoordinates[notFoundInGroundTruth],15)
    contoursStalledOrange = makeContoursList(frameCount, groundTruthCoordinates,10, True)  

    for i in range(frameCount):
        try:
            image.seek(i)
            imageRGB = image.convert('RGB')
            outputImage[i,:,:] = np.array(imageRGB) 
            for contours in contoursListRed[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (255,0,0), 2)
            for contours in contoursListBlue[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (0,0,255), 2)
            for contours in contoursListGreen[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (0,255,0), 2)
            for contours in contoursListYellow[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (255,255,0), 2)
            for contours in contoursStalledOrange[i]:
                cv2.drawContours(outputImage[i,:,:, :],contours , -1, (255,100,0), 2)                
        except EOFError:
            print("Error")
            # Not enough frames in img
            break

    print("Saving tif stack to %s" % outputImageName)
    imsave(outputImageName, outputImage)

