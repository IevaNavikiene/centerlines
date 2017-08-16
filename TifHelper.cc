//includes for tif helper
//include <vector>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vtkImageActor.h>
#include <vtkImageViewer2.h>
#include <vtkTIFFReader.h>
#include <vtkTIFFWriter.h>
#include <vtkImageMapper3D.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>

//include <vtkSmartPointer.h>
//include <vtkRenderWindow.h>
//include <vtkRenderWindowInteractor.h>
//include <vtkRenderer.h>
//include <iostream>
using namespace cv;
using namespace std;

class LineIterator {
public:
    // creates iterators for the line connecting pt1 and pt2
    // the line will be clipped on the image boundaries
    // the line is 8-connected or 4-connected
    // If leftToRight=true, then the iteration is always done
    // from the left-most point to the right most,
    // not to depend on the ordering of pt1 and pt2 parameters
    LineIterator(const Mat& img, Point pt1, Point pt2,
            int connectivity = 8, bool leftToRight = false);
    // returns pointer to the current line pixel
    uchar* operator*();
    // move the iterator to the next pixel
    LineIterator& operator++();
    LineIterator operator++(int);
    Point pos() const;

    // internal state of the iterator
    uchar* ptr;
    int err, count;
    int minusDelta, plusDelta;
    int minusStep, plusStep;
};


/* class to draw open, save, draw on stack of tif images*/
class TifHelpper {
private:

    vtkSmartPointer<vtkTIFFReader> readImage(string filename) {
        cout << "Starting to read tif image" << endl;
        //Read the image
        vtkSmartPointer<vtkTIFFReader> reader =
                vtkSmartPointer<vtkTIFFReader>::New();
        /*
                 if (!jpegReader->CanReadFile(inputFilename.c_str())) {
            cout << imageName.c_str() << ": Error reading file "
                    << "Exiting..." << endl;
            return;
        }
         */
        reader->SetFileName(filename.c_str());

        /*
                image = reader.GetOutput()
                        //For my test TIFF file of 640x400, I get the following
                        image.GetExtent()
                        (0, 639, 0, 399, 0, 0)

                        // So this was for a single image. For the volumetric image
                        // you now do something like the following

                        volume = vtkImageData()
                        volume.SetExtent((0, 639, 0, 399, 0, N - 1)#N  is the number of TIFF images

                        // Get the number of cells and points for your volume
                        ncells = volume.GetNumberOfCells()
                        npoints = volume.GetNumberOfPoints()
                        /*
                        The final piece of the puzzle is to get your TIFF data into the
                        volume image. This is actually quite simple. What you do
                        is create a data array to hold the data for each point
                        in your volume image The dimension is npoints. Than you loop
                        over each TIFF image and copy the data to the array. So something
                        like the following:
         *//*
        array = vtkUnsignedShortArray()
                array.SetNumberOfValues(npoints)

        for (int i(0); i < N; i++) {
            reader.SetFileName('test_%d.tiff' % i)
            reader.Update()
            img = reader.GetOutput()
                    vals = img.GetPointData().GetArray('Tiff Scalars')
                    offset = i * N
            for i, v in enumerate(vals) :
                array.SetValue(offset + i, v)
            }
        // Finally we have to assign our array to the volume image.
        volume.GetPointData().SetArray(array)
                */

        /*
                // Visualize
                vtkSmartPointer<vtkImageViewer2> imageViewer =
                        vtkSmartPointer<vtkImageViewer2>::New();
                imageViewer->SetInputConnection(reader->GetOutputPort());

                vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
                        vtkSmartPointer<vtkRenderWindowInteractor>::New();
                imageViewer->SetupInteractor(renderWindowInteractor);
                cout << "Starting to read tif image" << endl;
                imageViewer->Render();
                cout << "Starting to read tif image" << endl;
                // imageViewer->GetRenderer()->ResetCamera(); cout << "Starting to read tif image" << endl;
                //  imageViewer->Render();
                //cout << "Starting to read tif image" << endl;
                renderWindowInteractor->Start();*/
        return reader;
    }

    void writeImage(vtkSmartPointer<vtkTIFFReader> &tifReader, string filename) {

        cout << "Starting to write tif image" << endl;
        string outputFilename = "output_" + filename;
        vtkSmartPointer<vtkTIFFWriter> tiffWriter = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriter->SetFileName(outputFilename.c_str());
        tiffWriter->SetInputConnection(tifReader->GetOutputPort());
        tiffWriter->Write();
        // Read and display for verification
        vtkSmartPointer<vtkTIFFReader> reader =
                vtkSmartPointer<vtkTIFFReader>::New();
        reader->SetFileName(outputFilename.c_str());
        reader->Update();

        vtkSmartPointer<vtkImageActor> actor =
                vtkSmartPointer<vtkImageActor>::New();
        actor->GetMapper()->SetInputConnection(reader->GetOutputPort());

        // Setup renderer
        vtkSmartPointer<vtkRenderer> renderer =
                vtkSmartPointer<vtkRenderer>::New();
        renderer->AddActor(actor);
        renderer->ResetCamera();

        // Setup render window
        vtkSmartPointer<vtkRenderWindow> renderWindow =
                vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);

        // Setup render window interactor
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
                vtkSmartPointer<vtkRenderWindowInteractor>::New();
        vtkSmartPointer<vtkInteractorStyleImage> style =
                vtkSmartPointer<vtkInteractorStyleImage>::New();

        renderWindowInteractor->SetInteractorStyle(style);

        // Render and start interaction
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderWindowInteractor->Initialize();

        renderWindowInteractor->Start();
        cout << "Saved " << outputFilename.c_str() << " file with alpha data." << endl;
    }

    void drawLineOnImage(Mat &image) {

        cout << "Starting to draw on tif image" << image.size() << endl;
        vector<Point> contour;
        contour.push_back(Point(50, 50));
        contour.push_back(Point(10, 50));
        contour.push_back(Point(60, 100));
        contour.push_back(Point(80, 90));
        contour.push_back(Point(70, 80));
        contour.push_back(Point(60, 100));

        // create a pointer to the data as an array of points (via a conversion to 
        // a Mat() object)

        const cv::Point *pts = (const cv::Point*) Mat(contour).data;
        int npts = Mat(contour).rows;

        cout << "Number of polygon vertices: " << npts << std::endl;

        // draw the polygon 

        polylines(image, &pts, &npts, 1,
                true, // draw closed contour (i.e. joint end to start) 
                Scalar(0, 255, 0), // colour RGB ordering (here = green) 
                3, // line thickness
                CV_AA, 0);
        cout << "rrrrrrrrrr" << endl;
        namedWindow("Components", 1);
        cout << "ttttttttttt" << image.size() << endl;
        imshow("Components", image);
        cout << "wwwwwwwww" << endl;
        waitKey(0);
    }
    
    Mat convertPolydataToImageData(vtkTIFFReader &imageReader){
        Mat image;
        cout<<" getOutput: "<<imageReader->getOutput();
    }
public:

    void drawCenterlines(string imageName, string centerlinesFile) {

        vtkSmartPointer<vtkTIFFReader> imageReader = readImage(imageName);
        Mat image = convertPolydataToImageData(imageReader);
        cout << "image size:" << endl;
        //drawLineOnImage(image);
        //check if image is not empty
        string outputName = "output_" + imageName;
        writeImage(imageReader, outputName.c_str());
    }
};
