/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VtpHelpper.h
 * Author: ieva
 *
 * Created on February 1, 2017, 2:47 PM
 */

#ifndef VTPHELPPER_H
#define VTPHELPPER_H

#include <string>
//read vtp file and display it
#include <vtkOBJReader.h>
#include "vtkSmartPointer.h"
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataMapper.h>

using namespace std;

class VtpHelpper {
public:
    VtpHelpper();

    /**
     * Read vtp/obj file to check it it was saved correctly
     * @param filename
     */
    void readXmlFile(string filename);

    void visualize(vtkSmartPointer<vtkXMLPolyDataReader> &reader);

    void visualize(vtkSmartPointer<vtkOBJReader> &reader);

    /**
     * Read vtp file to check it it was saved correctly
     * @param filename
     * @param visualize
     */
    vtkSmartPointer<vtkXMLPolyDataReader> readVtpFile(string filename, bool visualize = false);

    /**
     * Read obj file to check it it was saved correctly
     * @param filename
     * @param visualize
     */
    void readObjFile(string filename, bool visualize = false);

    /**
     * Create a polydata object and add the points to it.
     * @param points
     * @param lines
     */
    vtkSmartPointer<vtkPolyData> createPolydataObject(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkCellArray> lines, vtkSmartPointer<vtkUnsignedCharArray> additionalData);


    /**
     * Write Polydata object to file
     * @param polydata
     * @param filename- filename.vtp or filename.obj
     */
    void createCsvFile(vtkSmartPointer<vtkPolyData> &polydata, string outputFilename);

    /**
     * Write Polydata object to file
     * @param polydata
     * @param filename- filename.vtp or filename.obj
     */
    void createVtpFile(vtkSmartPointer<vtkPolyData> &input, string outputFilename);


    /**
     * Filter centerlines from "stars" and short centerlines
     * which appeared due to the noise
     * @param polydata
     * @return 
     */
    vtkSmartPointer<vtkPolyData> filterShortCenterlines(vtkPolyData* polydata);

    /**
     * UNUSED!!!
     * Form centerlines objects from points 
     */
    vtkSmartPointer<vtkPolyData> filterCenterlinesFromStars(vtkPolyData* polydata);
private:
};

#endif /* VTPHELPPER_H */

