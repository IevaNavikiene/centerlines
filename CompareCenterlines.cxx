/*
 * Read Plist xml centerlines file generated in Schaffer – Nishimura Lab 
 * (https://snlab.bme.cornell.edu/) and convert it to vtp format so that
 * it could be compared with centerlines network generated in vmtk tool
 * 
 */

/* 
 * File:   CompareCenterlines.cpp
 * Author: ieva
 *
 * Created on December 20, 2016, 10:05 PM
 */

/*
 * Includes needed to convert plist to vtp
 */
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>  
#include <vector>
#include <array>
#include <fstream>

#include <vtkVersion.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

//lines and cells
#include <vtkLine.h>
#include <vtkPolyLine.h>

//Additional functionality 
//#include <ConvertToStl.h>
//Test
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <unistd.h>
//#include "prettyprint.hpp"
#include <typeinfo>

#include <vtkFieldData.h>
#include <vtkCellTypes.h>
//read vtp file and display it
#include "vtkOBJWriter.h"

#include "src/ConvertPlist.h"
#include "src/VtpHelpper.h"
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

template <typename T, unsigned S>
inline unsigned arraysize(const T(&v)[S]) {
    return S;
}

using namespace std;
const int TYPE_CONVERT_PLIST = 1;
const int TYPE_CONVERT_VTP_TO_OBJ = 2;
const int TYPE_FILTER_CENTERLINES = 3;
const int TYPE_CONVERT_VTP_TO_CSV = 4;

/*
 * Gets filenames from user what file to process and to what file output
 * result
 */
int main(int argc, char* argv[]) {
    // Parse command line arguments

    if (argc < 4) {
        //TODO write simple interface with choosing between two modes
        cout << "Usage if you want to convert xml file: " << argv[0] << " inputFilename(.xml) outputFilename(.vtp, .obj) 1(type)" << std::endl;
        cout << "Usage if you want to convert vtp file to obj: " << argv[0] << " inputFilename.vtp outputFilename.obj 2(type)" << std::endl;
        cout << "Get rid of 'stars' and too short centerlines: " << argv[0] << " inputCenterlines.vtp outputCenterlines.vtp 3(type)" << std::endl;
        // cout << "Draw centerlines on image stack: " << argv[0] << " centerlines.vtp tiffImage.tif 4(type)" << std::endl;
        cout << "Convert centerlines vtp to csv(which  I can read in python): " << argv[0] << " centerlines.vtp outputFile.csv 4(type)" << std::endl;
        cout << "Usage if you want to visualize .obj file: " << argv[0] << " filename1(.obj) filename2(.vtp) 5(type)" << std::endl;
        //cout << "Usage if you want to convert .obj file to vtp: " << argv[0] << " inputFilename.obj outputFilename.vtp 5(type)" << std::endl;
        return EXIT_FAILURE;
    }

    string inputFilename = argv[1];
    string outputFilename = argv[2];
    string type = argv[3];
    string outputFilenameString(outputFilename.c_str());
    /*
     string inputFilename = "test2.xml";
     string outputFilename = "ground11.obj";
     string type = "1";
  string outputFilenameString(outputFilename.c_str());
     *  */
    VtpHelpper vtpHelpper;
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> input =
            vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkXMLPolyDataReader> reader =
            vtkSmartPointer<vtkXMLPolyDataReader>::New();

    switch (stoi(type.c_str())) {
        case TYPE_CONVERT_PLIST:
        {
            ConvertPlist convertPlist;
            convertPlist.readFile(inputFilename);
            /*
             * Form centerlines objects from points 
             */
            convertPlist.insertLinesAndPoints();
            /*
             * Store centerlines in polydata object 
             */
            polydata = vtpHelpper.createPolydataObject(convertPlist.getPoints(), convertPlist.getLines(), convertPlist.getAdditionalData());
            input->ShallowCopy(polydata);
            break;
        }
        case TYPE_CONVERT_VTP_TO_CSV:
        case TYPE_CONVERT_VTP_TO_OBJ:
            reader = vtpHelpper.readVtpFile(inputFilename.c_str());
            input->ShallowCopy(reader->GetOutput());
            break;
        case TYPE_FILTER_CENTERLINES:
        {
            reader = vtpHelpper.readVtpFile(inputFilename.c_str());
            input->ShallowCopy(vtpHelpper.filterShortCenterlines(reader->GetOutput()));
            break;
        }
        default:
        {
            cout << "Type not found, no action taken" << endl;
            return EXIT_SUCCESS;
        }
    }

    if (outputFilenameString.substr(outputFilenameString.length() - 3) == "obj") {
        vtkSmartPointer<vtkOBJWriter> writer =
                vtkSmartPointer<vtkOBJWriter>::New();
        writer->SetInputData(input);
        writer->SetFileName(outputFilename.c_str());
        writer->Update();

        /*
         * Read OBJ file to re-check if everything wrote successfully
         */
        //There is error with this class linking to separate class so it is here (not tidy but working)
        //vtpHelpper.readObjFile(outputFilename.c_str(), true);
        vtkSmartPointer<vtkOBJReader> reader =
                vtkSmartPointer<vtkOBJReader>::New();
        reader->SetFileName(outputFilename.c_str());
        reader->Update();
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(reader->GetOutputPort());
        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        vtkSmartPointer<vtkRenderer> renderer =
                vtkSmartPointer<vtkRenderer>::New();
        renderer->AddActor(actor);
        renderer->SetBackground(.3, .6, .3); // Background color green

        vtkSmartPointer<vtkRenderWindow> renderWindow =
                vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
                vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renderWindowInteractor->SetRenderWindow(renderWindow);

        renderWindowInteractor->Start();

    } else if (outputFilenameString.substr(outputFilenameString.length() - 3) == "vtp") {
        /*
         * Store centerlines object in VTP format
         */
        vtpHelpper.createVtpFile(input, outputFilenameString);

        /*
         * Read VTP file to re-check if everything wrote successfully
         */
        vtpHelpper.readVtpFile(outputFilename.c_str(), true);
    } else if (outputFilenameString.substr(outputFilenameString.length() - 3) == "tif") {
        /*For drawing centerlines on stack of images*/
        //TifHelpper tifHelpper;
        //tifHelpper.drawCenterlines(outputFilename, inputFilename);
    } else if (outputFilenameString.substr(outputFilenameString.length() - 3) == "csv") {
        vtpHelpper.createCsvFile(input, outputFilename);
    }

    return EXIT_SUCCESS;
}
