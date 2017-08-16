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

# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required = True, help = "Path to the image")
ap.add_argument("-c", "--centerline", required = True, help = "Path to the centerline csv document")
args = vars(ap.parse_args())

outputImageName = "multipage2_thick2.tif"
# load the image, clone it for output, and then convert it to grayscale
image = Image.open(args['image'])

frameCount = image.n_frames
width, height = image.size
print(width, height,"width, height")
outputImage = np.zeros((frameCount, height, width,3 ), 'uint16')

#open csv document and form array to draw centerlines
def centerlinesCoordinates(filename):
    centerlinesCoordinates = []
    with open(filename, 'rb') as csvfile:
       spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
       for row in spamreader:
		   #print(len(row) ,"len(row) ",row[0])
           rowArray = row[0].split(',')
           if(len(rowArray) < 4):
               continue
               
           #convert coordinates to float
           rowArray[1] = int(math.floor(float(rowArray[1])));
           rowArray[2] = int(math.floor(float(rowArray[2])));
           rowArray[3] = int(math.floor(float(rowArray[3])))-77;#
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
				   centerlinesCoordinates.append([[[rowArray[1], rowArray[2]]],[rowArray[3], rowArray[3]], rowArray[4]])
               
    return  np.array(centerlinesCoordinates)
    
centerCoordinates = centerlinesCoordinates(args['centerline'])
'''
pts = np.array([[90,5],[20,30],[70,20],[50,10]], np.int32)
pts = pts.reshape((-1,1,2))
'''
#open_cv_image = open_cv_image[:, :, ::-1].copy() 
for i in range(frameCount):
    try:
        image.seek(i)
        imageRGB = image.convert('RGB')
        outputImage[i,:,:] = np.array(imageRGB) 
        #outputImage[i,:,:] = outputImage[i,:,:][i, :, :, ::-1].copy() 
        for centerlineCoord in centerCoordinates:
            #print("centerlineCoord[1][0] :",centerlineCoord[1][0] ,"centerlineCoord[1][1]= ",centerlineCoord[1][1] ,"i=",i)
            if(centerlineCoord[1][0] <= i and centerlineCoord[1][1]>=i):
                #centerlineCoord[0].astype(np.int32)
                convertedArray = np.array([np.array(xi, np.int32) for xi in centerlineCoord[0]])
                pts = convertedArray.reshape((-1,1,2))
                #cv2.drawContours(outputImage[i,:,:], [pts], 0, (255,255,255))
                #cv2.fillConvexPoly(outputImage[i,:,:], pts, (255,255,255))
                if(centerlineCoord[2] == '0'):
                    cv2.polylines(outputImage[i,:,:],[pts],False,(255,0,0),5)
                else:
					cv2.polylines(outputImage[i,:,:],[pts],False,(0,255,0),5)
        #cv2.imshow("Components", outputImage[i,:,:]);
        #cv2.waitKey(10);
        #print image.getpixel( (600,200))
    except EOFError:
        print("Error")
        # Not enough frames in img
        break

print("Saving tif stack to %s" % outputImageName)
imsave(outputImageName, outputImage)
'''
output = image.copy()
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# detect circles in the image
circles = cv2.HoughCircles(gray, cv2.cv.CV_HOUGH_GRADIENT, 1.2, 100)
 
# ensure at least some circles were found
if circles is not None:
    # convert the (x, y) coordinates and radius of the circles to integers
    circles = np.round(circles[0, :]).astype("int")
 
    # loop over the (x, y) coordinates and radius of the circles
    for (x, y, r) in circles:
        # draw the circle in the output image, then draw a rectangle
        # corresponding to the center of the circle
        cv2.circle(output, (x, y), r, (0, 255, 0), 4)
        cv2.rectangle(output, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)
 
    # show the output image
    cv2.imshow("output", np.hstack([image, output]))
    cv2.waitKey(0)
'''
