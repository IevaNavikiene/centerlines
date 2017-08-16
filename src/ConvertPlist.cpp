/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConvertPlist.cpp
 * Author: ieva
 * 
 * Created on February 1, 2017, 1:11 PM
 */

#include "ConvertPlist.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>  
#include <vector>
#include <array>

#include "XmlInspector.hpp"
#include "vtkSmartPointer.h"
#include <vtkPolyLine.h>

#include <vtkVersion.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
// For compatibility with new VTK generic data arrays
//#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
//#endif
 


using namespace std;

ConvertPlist::ConvertPlist() {
    this->vesselNum = 0;
    this->points = vtkSmartPointer<vtkPoints>::New();

    // Create a cell array to store the line in
    this->lines = vtkSmartPointer<vtkCellArray>::New();

    // Create a vtkUnsignedCharArray container and store the colors in it
    this->additionalData = vtkSmartPointer<vtkUnsignedCharArray>::New();
    /*
    private:
    typedef array<double, 3> Coord;

    struct Vessel {
        vector<Coord> linePathInMicrons;
        Coord micronEndPoint;
        Coord micronStartPoint;
    };
    int vesselNum = 0;
    Vessel Vessels[1218];

    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();

    // Create a cell array to store the line in
    vtkSmartPointer<vtkCellArray> lines =
            vtkSmartPointer<vtkCellArray>::New();

    public:

    Vessel getInitializedVessel() {
        vector<Coord> middle(0);
        Vessel ves = {middle, 0.0, 0.0};
        return ves;
    }
     */
}

ConvertPlist::Vessel ConvertPlist::getInitializedVessel() {
    vector<Coord> middle(0);
    Vessel ves = {middle, 0.0, 0.0};
    return ves;
}

void ConvertPlist::readFile(string filename) {
    /**
     * Read plist xml object
     * @param filename
     */
    cout << "filename:" << filename << endl;

    Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(filename);
    //indicate that it is time to search specific part (I read lines , but key and value is in different rows)
    array<bool, 4 > foundVessels = {false, false, false, false};
    //indicate that now numbers will be from specific part (I read lines and every x,y,z coordinate is in different row)
    array<bool, 4 > foundElement = {false, false, false, false};

    int elementToUpdate(0);
    //pointValue variable for converting each value to float from string
    string::size_type pointValue;
    Coord midCoords;
    long double coordArray;
    while (inspector.Inspect()) {

        if (inspector.GetInspected() == Xml::Inspected::Text && inspector.GetValue() == "Vessels") {
            foundVessels[0] = true;
        }
        if (foundElement[0] == true && foundVessels[1] == true && inspector.GetValue() == "micronEndPoint") {
            foundElement[0] = false;
        }

        if (foundElement[0] == true && inspector.GetInspected() == Xml::Inspected::Text) {
            string input = inspector.GetValue();
            istringstream ss(input);
            string token;
            int ii(0);
            while (std::getline(ss, token, ',')) {
                midCoords[ii] = stold(token, &pointValue);
                ii++;
            }
  
            if (midCoords[1] > 503.0) {
                //    if (this->vesselNum >= this->Vessels->size()) {
                //       (this->Vessels).push_back(getInitializedVessel());
                //    }
                (this->Vessels[this->vesselNum].linePathInMicrons).push_back({midCoords[0], midCoords[1]-503.0, midCoords[2]});
            }
        } else if (inspector.GetInspected() == Xml::Inspected::Text && (foundElement[1] == true || foundElement[2] == true)) {
            coordArray = stold(inspector.GetValue(), &pointValue);
            //  cout << "coordArray" << coordArray << endl;

            /* if height coordinate is bigger then change it to -1 and later the point won't be saved in array*/
            if (elementToUpdate == 1 and coordArray <= 503.0) {
                coordArray = -1.0;
            }else if(elementToUpdate == 1){
                coordArray = coordArray-503.0;
            }
            if (foundElement[1] == true) {
                this->Vessels[this->vesselNum].micronEndPoint[elementToUpdate] = coordArray;
            } else {
                this->Vessels[this->vesselNum].micronStartPoint[elementToUpdate] = coordArray;
            }
            //  exit(2);
            //in each row there is next coordinate so elementToUpdate element tells which array place to update
            if (elementToUpdate == 2) {
                elementToUpdate = 0;
                if (foundElement[1] == true) {
                    foundElement[1] = false;
                } else {
                    foundElement[2] = false;
                }
            } else {
                elementToUpdate += 1;
            }

        } else if (inspector.GetInspected() == Xml::Inspected::Text && foundElement[3] == true) {
            // Check if vessel is stalled
            bool stalled = false;
            if (inspector.GetValue() == "Stalled") {
                cout<<"ssssssssssssssssssstalled"<<endl;
                stalled = true;
            }
            this->Vessels[this->vesselNum].stalled = stalled;
            this->vesselNum++; //increase vessel because it is the last thing found in plist vessel
            foundElement[3] = false;
        }
        
        
        if (foundVessels[0] == true && inspector.GetValue() == "linePathInMicrons") {
            foundElement[0] = true;
            foundVessels[0] = false;
            foundVessels[1] = true;
        } else if (foundVessels[1] == true && inspector.GetValue() == "micronEndPoint") {
            foundElement[1] = true;
            foundVessels[1] = false;
            foundVessels[2] = true;
        } else if (foundVessels[2] == true && inspector.GetValue() == "micronStartPoint") {
            foundElement[2] = true;
            foundVessels[2] = false;
            foundVessels[3] = true;
        } else if (foundVessels[3] == true && inspector.GetValue() == "stalledState") {
            foundElement[3] = true;//mark element that now we search for value (without this element logic would fail
            foundVessels[3] = false; // mark that this element is already found
            foundVessels[0] = true; //mark that next element to search is first one
        }
    }
}

void ConvertPlist::insertLinesAndPoints() {
    /**
     * Form centerlines objects from points 
     */

    int pointsCount(0);
    // Create a line between the two points
    vtkSmartPointer<vtkPolyLine> line =
            vtkSmartPointer<vtkPolyLine>::New();

    // Create two colors - red for stalled vessels and green for not stalled
    unsigned char red[3] = {255, 0, 0};
    unsigned char green[3] = {0, 255, 0};

    this->additionalData->SetNumberOfComponents(vesselNum);
    this->additionalData->SetName("stalled_colors");
    //vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
    for (vtkIdType i = 0; i < vesselNum; i++) {
        int additionalPointsSize((this->Vessels[i].linePathInMicrons).size());
        //you have to set how many points will be connected here
        line->GetPointIds()->SetNumberOfIds(2 + additionalPointsSize);
       
        if (this->Vessels[i].stalled == true) {
            this->additionalData->InsertNextTypedTuple(red);
        } else {
            this->additionalData->InsertNextTypedTuple(green);
        }
        int pointInLine(0);
        if (this->Vessels[i].micronStartPoint[1] != -1) {
            this->points->InsertNextPoint(this->Vessels[i].micronStartPoint[0], this->Vessels[i].micronStartPoint[1], this->Vessels[i].micronStartPoint[2]);
            line->GetPointIds()->SetId(pointInLine, pointsCount); //the SetId(A,B) call is the following: 
            pointsCount++;
            pointInLine++;
        }


        for (int num(0); num < additionalPointsSize; num++) {
            this->points->InsertNextPoint(Vessels[i].linePathInMicrons[num][0], Vessels[i].linePathInMicrons[num][1], Vessels[i].linePathInMicrons[num][2]);
            line->GetPointIds()->SetId(pointInLine, pointsCount);
            pointInLine++;
            pointsCount++;
        }
        if (this->Vessels[i].micronEndPoint[1] != -1) {
            this->points->InsertNextPoint(this->Vessels[i].micronEndPoint[0], this->Vessels[i].micronEndPoint[1], this->Vessels[i].micronEndPoint[2]);
            //     std::cout << "Start Point " << i << " : (" << this->Vessels[i].micronStartPoint[0] << " " << this->Vessels[i].micronStartPoint[1] << " " << this->Vessels[i].micronStartPoint[2] << ")" << std::endl;
            //   std::cout << "End Point " << i << " : (" << this->Vessels[i].micronEndPoint[0] << " " << this->Vessels[i].micronEndPoint[1] << " " << this->Vessels[i].micronEndPoint[2] << ")" << std::endl;
            // exit(2);
            /*****************Add lines and cells ********************************/
            line->GetPointIds()->SetId(pointInLine, pointsCount);
            this->lines->InsertNextCell(line);

            pointsCount++;
        }

        /*****************End of Add lines and cells ********************************/

    }
}

vtkSmartPointer<vtkPoints> ConvertPlist::getPoints() {
    return this->points;
}

vtkSmartPointer<vtkCellArray> ConvertPlist::getLines() {
    return this->lines;
}

vtkSmartPointer<vtkUnsignedCharArray> ConvertPlist::getAdditionalData() {
    return this->additionalData;
}
