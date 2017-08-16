/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConvertPlist.h
 * Author: ieva
 *
 * Created on February 1, 2017, 1:11 PM
 */

#ifndef CONVERTPLIST_H
#define CONVERTPLIST_H

#include <array>
#include <vector>
#include <string>
#include "vtkPolyDataAlgorithm.h" //superclass
#include <vtkUnsignedCharArray.h>

using namespace std;

class ConvertPlist {
public:
    typedef array<double, 3> Coord;

    struct Vessel {
        vector<Coord> linePathInMicrons;
        Coord micronEndPoint;
        Coord micronStartPoint;
        bool stalled;
    };
    ConvertPlist();

    Vessel getInitializedVessel();

    void readFile(string filename);

    void insertLinesAndPoints();

    vtkSmartPointer<vtkPoints> getPoints();

    vtkSmartPointer<vtkCellArray> getLines();
    
    vtkSmartPointer<vtkUnsignedCharArray> getAdditionalData();
private:
    int vesselNum;
    Vessel Vessels[10000];

    vtkSmartPointer<vtkPoints> points;

    // Create a cell array to store the line in
    vtkSmartPointer<vtkCellArray> lines;
    
    // Create additional data array where stalled property is saved as line color
    vtkSmartPointer<vtkUnsignedCharArray>additionalData;
};

#endif /* CONVERTPLIST_H */

