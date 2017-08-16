# import the necessary packages
import numpy as np
import argparse
import cv2
from PIL import Image
import time
import csv
from tifffile import imsave
from matplotlib import pyplot as plt
'''
Apply filters to image:
1. 2D gaussian blur with ksize= 5, sigma=0.3*((ksize-1)*0.5 - 1) + 0.8 
2. Adaptive local threshold (ADAPTIVE_THRESH_GAUSSIAN_C radius 11 and C value=-1 )
3. medianBlur to reduce salt and pepper noise
4. Adaptive threshold THRESH_OTSU
'''

# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required = True, help = "Path to the image")
args = vars(ap.parse_args())

outputImageName = "filtered_" + args['image']
# load the image, clone it for output, and then convert it to grayscale
image = Image.open(args['image'])

frameCount = image.n_frames
width, height = image.size
print(width, height,"width, height")
outputImage = np.zeros((frameCount, height, width), 'uint8')

'''
Find out bad quality regeons
150*150 with 50 step
'''
regionSize = 300
stepSize = 100
regeonsList = []
regeonsX = ((width-regionSize)/stepSize)+1
regeonsY = ((height-regionSize)/stepSize)+1
print(regeonsX,"regeonsX",regeonsY,"regeonsY")
for x in range(regeonsX):
    for y in range(regeonsY):
        regeonsList.append([[x*stepSize,x*stepSize+regionSize],[y*stepSize,y*stepSize+regionSize]])
if((x*stepSize+regionSize) < width or (y*stepSize+regionSize) < height):
    regeonsList.append([[width-regionSize,width],[height -regionSize,height]])
regeonsToMakeBlack= np.zeros((frameCount,len(regeonsList)))

'''
Draw contours and save image
'''
for i in range(frameCount):
    try:
        image.seek(i)
        outputImage[i,:,:] = np.array(image) 
        #2D gaussian blur
        #outputImage[i,:,:] = cv2.GaussianBlur(outputImage[i,:,:],(5,5),0)
        #outputImage[i,:,:] = cv2.adaptiveThreshold(outputImage[i,:,:],255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,11,-1)
        for num in range(len(regeonsList)):
            averagePixelIntensity = np.sum(outputImage[i,regeonsList[num][1][0]:regeonsList[num][1][1],regeonsList[num][0][0]:regeonsList[num][0][1]])/(255*regionSize* regionSize)
            print(averagePixelIntensity,"averagePixelIntensity")
            if(averagePixelIntensity> 0.19 or averagePixelIntensity < 0.02):
                regeonsToMakeBlack[i][num] = 1
        outputImage[i,:,:] = cv2.medianBlur(outputImage[i,:,:],5)
        #_,outputImage[i,:,:] = cv2.threshold(outputImage[i,:,:],0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    except EOFError:
        print("Error")
        # Not enough frames in img
        break
        
#delete noisy regeons from movie start
deletedRegeonsToMakeBlack= np.zeros((frameCount,len(regeonsList)))
deletedRegeonsToMakeBlack[0] = regeonsToMakeBlack[0]
for i in range(frameCount):
    for num in range(len(regeonsList)):
        #if it is the last or first frame or previous frame was black
        if(regeonsToMakeBlack[i][num] == 1 and (i == 0 or deletedRegeonsToMakeBlack[i-1][num] == 1)):
            outputImage[i, regeonsList[num][1][0]:regeonsList[num][1][1],regeonsList[num][0][0]:regeonsList[num][0][1]] = np.zeros((regionSize,regionSize), 'uint8')
            deletedRegeonsToMakeBlack[i][num] = 1
#delete noisy regeons from movie end
deletedRegeonsToMakeBlackEnd= np.zeros((frameCount,len(regeonsList)))
deletedRegeonsToMakeBlackEnd[0] = regeonsToMakeBlack[0]
for i in reversed(range(frameCount)):
    for num in range(len(regeonsList)):
        #if it is the last or first frame or previous frame was black
        if(regeonsToMakeBlack[i][num] == 1 and ((i == (frameCount-1)) or deletedRegeonsToMakeBlackEnd[i + 1][num] == 1)):
            outputImage[i, regeonsList[num][1][0]:regeonsList[num][1][1],regeonsList[num][0][0]:regeonsList[num][0][1]] = np.zeros((regionSize,regionSize), 'uint8')
            deletedRegeonsToMakeBlackEnd[i][num] = 1
minLineLength = round(width/3)
outputImage1 = np.zeros((height, width), 'uint8')

# reduce noise in the image edges (asume that it is 60 pixels from edge)
nearEdge = 60#60
additionalPoints = 10#5
timeRange = 10
for i in range(frameCount):
    #i = 300
    lines = cv2.HoughLinesP(image=outputImage[i, :,:], rho=1, theta=np.pi/180, threshold= 100, lines=np.array([]), minLineLength=minLineLength, maxLineGap=80)
    if(lines is not None):
        a,b,c = lines.shape
        for n in range(a):
            if((lines[n][0][2] >= (width - nearEdge) or lines[n][0][2] <= nearEdge) and (lines[n][0][1] >= (height - nearEdge) or lines[n][0][1] <= nearEdge)):
                xLowerPoint = lines[n][0][2]
                xHigherPoint = lines[n][0][0]
                yLowerPoint = lines[n][0][3]
                yHigherPoint = lines[n][0][1]
                if(lines[n][0][2] > lines[n][0][0]):
                    xLowerPoint = lines[n][0][0]
                    xHigherPoint = lines[n][0][2]            
                if(lines[n][0][3]> lines[n][0][1]):
                    yLowerPoint = lines[n][0][1]
                    yHigherPoint = lines[n][0][3]
                    
                isHorizontal = True if((xHigherPoint-xLowerPoint) > (yHigherPoint-yLowerPoint)) else False
                if(isHorizontal == True): 
                    print("Horizontallll")   
                    xStart = xLowerPoint
                    xEnd =xHigherPoint
                    #if it is above middle image I paint black all area above or vice versa
                    if(yHigherPoint > height/2):
                        yStart = yLowerPoint-additionalPoints
                        yEnd = height
                    else:
                        yStart = 0
                        yEnd = yHigherPoint+additionalPoints
                else:
                    yStart = yLowerPoint
                    yEnd = yHigherPoint
                    if(xHigherPoint > width/2):
                        xStart = xLowerPoint-additionalPoints
                        xEnd = width
                    else:
                        xStart = 0
                        xEnd = xHigherPoint+additionalPoints
                    #xStart = xLowerPoint-10 if(xLowerPoint-10 >= 0) else 0
                    #xEnd =xHigherPoints+10 if(xHigherPoints+10 <= width) else width

                zStart = i-timeRange if((i-timeRange) >= 0) else 0
                zEnd = i+timeRange if((i+timeRange) < frameCount) else frameCount
                print("x",xStart,xEnd,"y:", yStart,yEnd,"z", zStart, zEnd)
                outputImage[zStart:zEnd, yStart:yEnd, xStart:xEnd] = np.zeros((zEnd-zStart,yEnd-yStart, xEnd-xStart), 'uint8')
                #cv2.line(outputImage1[:,:], (lines[n][0][0], lines[n][0][1]), (lines[n][0][2], lines[n][0][3]), 255, 1, cv2.LINE_AA)
                #break
        #plt.imshow(outputImage1[:,:])
        #plt.imshow(outputImage[i, :,:])
        #plt.show()
        #cv2.imshow('frame',outputImage1[:,:])
        #cv2.waitKey(0)

#Last time go through the movie and search frames where almost all regeons was deleted
# or if there exist not black screen between black ones
for i in range(frameCount):
    averagePixelIntensity = np.sum(outputImage[i,:,:])/(255*width* height)
    print(averagePixelIntensity,"averagePixelIntensity")
    zStart = i-5 if((i-5)>0) else 0
    if(averagePixelIntensity < 0.01):
        outputImage[zStart:i, :, :] = np.zeros((i-zStart,height, width), 'uint8')


print("Saving tif stack to %s" % outputImageName)
imsave(outputImageName, outputImage)
cv2.destroyAllWindows()
