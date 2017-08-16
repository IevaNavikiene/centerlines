/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VtpHelpper.cpp
 * Author: ieva
 * 
 * Created on February 1, 2017, 2:47 PM
 */

#include "VtpHelpper.h"
#include <vector>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellData.h>
#include "vtkSmartPointer.h"









#include <vtkDataArrayAccessor.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkXMLPolyDataWriter.h>

#include <vector>
#include <string>
using namespace std;

VtpHelpper::VtpHelpper() {
}

void VtpHelpper::readXmlFile(string filename) {
    /**
     * Read vtp/obj file to check it it was saved correctly
     * @param filename
     *
    cout << "filename.c_str()" << filename.c_str() << endl;
    // Read 
    vtkSmartPointer<vtkXMLPolyDataReader> reader =
            vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(.3, .6, .3); // Background color green

    renderWindow->Render();
    renderWindowInteractor->Start();*/
    //return reader;
}

void VtpHelpper::visualize(vtkSmartPointer<vtkXMLPolyDataReader> &reader) {
    // Visualize
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
}

void VtpHelpper::visualize(vtkSmartPointer<vtkOBJReader> &reader) {
    // Visualize
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
}

vtkSmartPointer<vtkXMLPolyDataReader> VtpHelpper::readVtpFile(string filename, bool visualize) {
    /**
     * Read vtp file to check it it was saved correctly
     * @param filename
     * @param visualize
     */
    vtkSmartPointer<vtkXMLPolyDataReader> reader =
            vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    if (visualize == true) {
        this->visualize(reader);
    }
    return reader;
}

void VtpHelpper::readObjFile(string filename, bool visualize) {
    /**
     * Read obj file to check it it was saved correctly
     * @param filename
     * @param visualize
     */
    /*
        vtkSmartPointer<vtkOBJReader> reader =
                vtkSmartPointer<vtkOBJReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();
        if (visualize == true) {
            this->visualize(reader);
        }
     */
}

vtkSmartPointer<vtkPolyData> VtpHelpper::createPolydataObject(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkCellArray> lines, vtkSmartPointer<vtkUnsignedCharArray> additionalData) {
    /**
     * Create a polydata object and add the points to it.
     * @param points
     * @param lines
     */
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    // Add the lines to the dataset
    polydata->SetLines(lines);
    // Add more data like stalled property
    polydata->GetCellData()->AddArray(additionalData);
    return polydata;
}

void VtpHelpper::createCsvFile(vtkSmartPointer<vtkPolyData> &polydata, string outputFilename) {
    /**
     * Write Polydata object to file
     * @param polydata
     * @param filename- filename.vtp or filename.obj
     */

    ofstream centerlines;
    centerlines.open(outputFilename.c_str());

    vtkCellArray *cells = polydata->GetLines();
    vtkIdType *indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;
    vtkPoints *points = polydata->GetPoints();



    vtkDataArray *annotationsArray = polydata->GetCellData()->GetArray(0); //GetAbstractArray("stalled_colors");
    //   cout << "annotationsArray->GetNumberOfTuples()" << annotationsArray->GetNumberOfTuples() << endl;
    //vtkDataArrayAccessor<vector> v(annotationsArray);
    // for (vtkIdType j = 0; j < annotationsArray->GetNumberOfTuples(); j++) {
    //    double testDouble = annotationsArray->GetTuple1(j);
    //     cout << j << "uuuuuuuuuuuuuu" << testDouble << endl;
    //  }



    for (cells->InitTraversal();
            cells->GetNextCell(numberOfPoints, indices);
            lineCount++) {
        cout << "Line " << lineCount << ": " << numberOfPoints << endl;
        double stalled = annotationsArray->GetTuple1(lineCount);
        for (vtkIdType i = 0; i < numberOfPoints; i++) {
            double point[3];
            points->GetPoint(indices[i], point);
           
            cout << "\t("
                    << point[0] << ", "
                    << point[1] << ", "
                    << point[2] << ")" << std::endl;
            centerlines << lineCount << "," << point[0] << "," << point[1] << "," << point[2] << "," << stalled << "\n";
        }
    }
    /*
       vtkCellData *cellData = polydata->GetCellData();
       if (cellData) {
           cout << " contains cell data with "
                   << cellData->GetNumberOfArrays()
                   << " arrays." << endl;
           cout << "Work with stalled annotations" << endl;
           vtkDataArray *annotationsArray = cellData->GetArray(0); //GetAbstractArray("stalled_colors");
           cout << "annotationsArray->GetNumberOfTuples()" << annotationsArray->GetNumberOfTuples() << endl;
           //vtkDataArrayAccessor<vector> v(annotationsArray);
           for (vtkIdType j = 0; j < annotationsArray->GetNumberOfTuples(); j++) {
               double testDouble = annotationsArray->GetTuple1(j);
               //cout << j << "uuuuuuuuuuuuuu" << testDouble << endl;
           }
       }


   

           std::cout << "Normals: " << polydata->GetPointData()->GetNormals() << std::endl;

           vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays();
           std::cout << "Number of PointData arrays: " << numberOfPointArrays << std::endl;

           vtkIdType numberOfCellArrays = polydata->GetCellData()->GetNumberOfArrays();
           std::cout << "Number of CellData arrays: " << numberOfCellArrays << std::endl;

           std::cout << "Type table/key: " << std::endl;
           ;
           //more values can be found in <VTK_DIR>/Common/vtkSetGet.h
    
           std::cout << VTK_UNSIGNED_CHAR << " unsigned char" << std::endl;
           std::cout << VTK_UNSIGNED_INT << " unsigned int" << std::endl;
           std::cout << VTK_FLOAT << " float" << std::endl;
           std::cout << VTK_DOUBLE << " double" << std::endl;

           for (vtkIdType i = 0; i < numberOfPointArrays; i++) {
               // The following two lines are equivalent
               //arrayNames.push_back(polydata->GetPointData()->GetArray(i)->GetName());
               //arrayNames.push_back(polydata->GetPointData()->GetArrayName(i));
               int dataTypeID = polydata->GetPointData()->GetArray(i)->GetDataType();
               std::cout << "Array " << i << ": " << polydata->GetPointData()->GetArrayName(i)
                       << " (type: " << dataTypeID << ")" << std::endl;
           }

           for (vtkIdType i = 0; i < numberOfCellArrays; i++) {
               // The following two lines are equivalent
               //polydata->GetPointData()->GetArray(i)->GetName();
               //polydata->GetPointData()->GetArrayName(i);
               int dataTypeID = polydata->GetCellData()->GetArray(i)->GetDataType();
               std::cout << "Array " << i << ": " << polydata->GetCellData()->GetArrayName(i)
                       << " (type: " << dataTypeID << ")" << std::endl;
           }
     */
    centerlines.close();

}

void VtpHelpper::createVtpFile(vtkSmartPointer<vtkPolyData> &input, string outputFilename) {
    /**
     * Write Polydata object to file
     * @param polydata
     * @param filename- filename.vtp or filename.obj
     */

    vtkSmartPointer<vtkXMLPolyDataWriter> writer =
            vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetFileName(outputFilename.c_str());
    writer->SetInputData(input);
    writer->Write();

}

vtkSmartPointer<vtkPolyData> VtpHelpper::filterShortCenterlines(vtkPolyData* polydata) {
    /**
     * Filter centerlines from "stars" and short centerlines
     * which appeared due to the noise
     * @param polydata
     * @return 
     */
    vtkCellArray *cells = polydata->GetLines();
    vtkIdType *indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;
    vtkSmartPointer<vtkPolyData> newPolydata =
            vtkSmartPointer<vtkPolyData>::New();

    newPolydata->DeepCopy(polydata);
    vector <int> cellsToDelete;
    int cellId(0);
    //from cell data get each centerline length and mark short centerlines to delete
    vtkCellData *cellData = polydata->GetCellData();
    if (cellData) {
        cout << " contains cell data with "
                << cellData->GetNumberOfArrays()
                << " arrays." << endl;
        /*
        for (int i = 0; i < cellData->GetNumberOfArrays(); i++) {
            cout << "\tArray " << i
                    << " is named "
                    << (cellData->GetArrayName(i) ? cellData->GetArrayName(i) : "NULL")
                    << endl;
        }*/
        cout << "Work with centerlines length... (if something went wrong check if you used this command line to generate lengths of centerlines:"
                << " vmtkcenterlinegeometry -ifile inputFile.vtp -ofile outputFile.vtp)" << endl;
        vtkDataArray *lengthArray = cellData->GetArray("Length");
        for (int j = 0; j < lengthArray->GetNumberOfTuples(); j++) {
            double value = lengthArray->GetTuple1(j);
            //If centerline is shorter than 8 units that means that it is more similar to noise
            if (value < 8.0) {
                cout << "  value " << j << "th is " << value << endl;
                cellsToDelete.push_back(cellId);
            }
            cellId++;
        }
    }

    cellId = 0;
    for (cells->InitTraversal();
            cells->GetNextCell(numberOfPoints, indices);
            lineCount++) {
        if (numberOfPoints <= 2) {
            cellsToDelete.push_back(cellId);
        }
        cellId++;
    }

    newPolydata->BuildLinks();
    for (auto & cellId : cellsToDelete) {
        // Mark a cell as deleted.
        cout << "delete: " << cellId << endl;
        newPolydata->DeleteCell(cellId);
    }
    // Remove the marked cell.
    newPolydata->RemoveDeletedCells();

    cout << "Finished short centerlines filter" << endl;
    return newPolydata;
}

vtkSmartPointer<vtkPolyData> VtpHelpper::filterCenterlinesFromStars(vtkPolyData* polydata) {
    /**
     * UNUSED!!!
     * Form centerlines objects from points 
     */
    vtkCellArray *cells = polydata->GetLines();
    vtkIdType *indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;
    /*vtkPoints *points = polydata->GetPoints();
    unsigned int pointInLine;
    unsigned int pointsCount(0);*/
    vtkSmartPointer<vtkPolyData> newPolydata =
            vtkSmartPointer<vtkPolyData>::New();

    newPolydata->DeepCopy(polydata);
    vector <int> cellsToDelete;
    int cellId(0);
    for (cells->InitTraversal();
            cells->GetNextCell(numberOfPoints, indices);
            lineCount++) {
        //if there are only two points in centerline perhaps it is the "star" and we should remove that centerline cell
        if (numberOfPoints <= 2) {
            cellsToDelete.push_back(cellId);
        }
        /*
        pointInLine = 0;
        cout << "Line " << lineCount << ": " << numberOfPoints << " indices: " << indices << endl;
        for (vtkIdType i = 0; i < numberOfPoints; i++) {
            double point[3];
            points->GetPoint(indices[i], point);
            cout << "\t("
                    << point[0] << ", "
                    << point[1] << ", "
                    << point[2] << ")" << std::endl;
            pointInLine++;
            pointsCount++;
        }*/
        cellId++;
    }

    newPolydata->BuildLinks();
    for (auto & cellId : cellsToDelete) {
        // Mark a cell as deleted.
        cout << "delete: " << cellId << endl;
        newPolydata->DeleteCell(cellId);
    }
    // Remove the marked cell.
    newPolydata->RemoveDeletedCells();

    /*
    cout << "start playing with triangle" << endl;

    //Setup point coordinates
    vtkSmartPointer<vtkPoints> pointss = vtkSmartPointer<vtkPoints>::New();
    pointss->InsertNextPoint(1.0, 0.0, 0.0);
    pointss->InsertNextPoint(0.0, 0.0, 1.0);
    pointss->InsertNextPoint(0.0, 0.0, 1.1);
    pointss->InsertNextPoint(0.0, 0.0, 0.0);
    pointss->InsertNextPoint(0.0, 1.1, 0.0);
    pointss->InsertNextPoint(0.0, 1.2, 0.0);
    pointss->InsertNextPoint(0.0, 1.2, 0.0);

    //create a line between each pair of points
    vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();

    line0->GetPointIds()->SetId(0, 0);
    line0->GetPointIds()->SetId(1, 1);
    vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
    line1->GetPointIds()->SetId(0, 1);
    line1->GetPointIds()->SetId(1, 2);

    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
    line2->GetPointIds()->SetId(0, 2);
    line2->GetPointIds()->SetId(1, 0);

    //create a cell array to store the line in
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);

    //create a polydata to store everything in
    vtkSmartPointer<vtkPolyData> polydataa = vtkSmartPointer<vtkPolyData>::New();

    //add the points and lines to the polydata
    polydataa->SetPoints(points);
    polydataa->SetLines(lines);
    /*
    vtkIdType connectivity[2];
    connectivity[0] = 0;
    connectivity[1] = 3;
    connectivity[2] = 2;
    connectivity[3] = 4;
    // for (int a(0);a<connectivity.length();a++) {
    cout << "conn1: " << connectivity[0] << endl;
    cout << "conn1: " << connectivity[1] << endl;
    cout << "conn1: " << connectivity[2] << endl;
    cout << "conn1: " << connectivity[3] << endl;
    //  }
    cout << " tipas "<<typeid(connectivity).name() <<" "<< '\n';
    polydataa->InsertNextCell(VTK_LINE, 4, connectivity); //Connects the first and fourth point we inserted into a line
         
            //write the polydata to a file
            vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
            writer->SetFileName("TriangleLines.vtp");
    #if VTK_MAJOR_VERSION <= 5
            writer->SetInput(polydata);
    #else
            writer->SetInputData(polydataa);
    #endif
            writer->Write();
     */
    cout << "Finished filter stars" << endl;
    return newPolydata;
}

