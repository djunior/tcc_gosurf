#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "timestack.h"

namespace tcc {

using namespace cv;
using namespace std;

Timestack::Timestack(VideoCapture& cap):
    size(cap.get(CV_CAP_PROP_FRAME_WIDTH)),
    timestack(0,size,CV_8UC1,0.0f)
{
    horizontalPercent = 50;
}

void Timestack::process(Mat& frame) {
    Mat greyMat, row, transposedMat;

    // Getting the frame width
    int width = frame.cols;

    float percent = (float) horizontalPercent / 100;

    int r = width * percent;
    if (r == width)
        r--;

    // Converting input to grey scale
    cvtColor(frame, greyMat, CV_BGR2GRAY);

    // cout << "Row: " << r << endl;
    // imshow("frame",greyMat);
    // waitKey(1);

    // Getting middle row from grey scale
    row = greyMat.row(r);

    // Transposing the row
    // transpose(row,transposedMat);

    // Pushing the transposed row to the timestack
    timestack.push_back(row);
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

        process(frame);
    }

}

void Timestack::setHorizontalPercent(int p) {
    horizontalPercent = p;
}

int Timestack::getHorizontalPercent() {
    return horizontalPercent;
}

void Timestack::save(std::string fileName) {
    Mat transposedTimestack;
    transpose(timestack,transposedTimestack);

    // imshow(fileName.c_str(),transposedTimestack);
    // waitKey(0);
    imwrite( fileName.c_str(), transposedTimestack );
}

}
