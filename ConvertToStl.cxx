#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

class ConvertToStl {
public:

    void convertVtpToStl(std::string inputFilename, std::string outputFilename) {
        // Read all the data from the file
        vtkSmartPointer<vtkXMLPolyDataReader> reader =
                vtkSmartPointer<vtkXMLPolyDataReader>::New();
        reader->SetFileName(inputFilename.c_str());
        reader->Update();
        vtkPolyData* polydataPoints1 = reader->GetOutput();
        cout << "polydataPoints1->GetNumberOfPoints(): " << polydataPoints1->GetNumberOfPoints() << endl;
        for (vtkIdType i = 0; i < polydataPoints1->GetNumberOfPoints(); i++) {
            double p[3];
            polydataPoints1->GetPoint(i, p);
        }
        for (vtkIdType i = 0; i < polydataPoints1->GetNumberOfPoints(); i++) {
            double p[3];
            polydataPoints1->GetPoint(i, p);
        }

        // Write the stl file
        vtkSmartPointer<vtkXMLPolyDataWriter> writer =
                vtkSmartPointer<vtkXMLPolyDataWriter>::New();
        writer->SetFileName(outputFilename.c_str());
        writer->SetInputData(polydataPoints1);
        writer->Write();
    }
};

