#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "timestack.h"

namespace tcc {

using namespace cv;
using namespace std;

Timestack::Timestack(VideoCapture& cap):
    size(cap.get(CV_CAP_PROP_FRAME_HEIGHT)),
    timestack(0,size,CV_8UC1,0.0f)
{
}

void Timestack::addFrame(Mat& frame) {
    Mat greyMat, middleRow, transposedMat;

    // Getting the frame width
    int width = frame.cols;

    // Converting input to grey scale
    cvtColor(frame, greyMat, CV_BGR2GRAY);

    // Getting middle row from grey scale
    middleRow = greyMat.col(width/2);

    // Transposing the row
    transpose(middleRow,transposedMat);

    // Pushing the transposed row to the timestack
    timestack.push_back(transposedMat);
}

Mat* Timestack::getTimestack() {
    return &timestack;
}

void Timestack::calculateTimestack(std::string videoPath)
{
    VideoCapture cap(videoPath); // open the video file for reading

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return;
    }

    while(true)
    {
        for (int i = 0; i < NUM_FRAMES_TO_SKIP; i++) {
            cap.grab();
        }

        Mat frame;
        bool bSuccess = cap.retrieve(frame);

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }

        addFrame(frame);
    }

}

void Timestack::save(std::string fileName) {
    Mat transposedTimestack;
    transpose(timestack,transposedTimestack);

    imwrite( fileName.c_str(), transposedTimestack );
}

}
