#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <algorithm>

#include "timestack.h"

namespace tcc {

using namespace cv;
using namespace std;

Timestack::Timestack(VideoCapture& cap)
{
    // cout << "WIDTH: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
    // cout << "HEIGHT: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
    // cout << "SIZE: " << size << endl;

    size = min(cap.get(CV_CAP_PROP_FRAME_WIDTH),cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    timestack = Mat(0,size,CV_8UC1,0.0f);

    horizontalPercent = 50;
}

void Timestack::process(Mat& frame) {
    Mat greyMat, row, transposedMat;

    // Getting the frame width
    int width = max(frame.cols,frame.rows);

    float percent = (float) horizontalPercent / 100;

    int r = width * percent;
    if (r == width)
        r--;

    // Converting input to grey scale
    // cvtColor(frame, greyMat, CV_BGR2GRAY);

    // cout << "Row: " << r << endl;
    // imshow("frame",greyMat);
    // waitKey(1);

    // cout << "rows: " << frame.rows << endl;
    // cout << "cols: " << frame.cols << endl;

    // Getting middle row from grey scale
    if (frame.cols > frame.rows) {
        row = frame.col(r);
        transpose(row,transposedMat); 
        timestack.push_back(transposedMat);
    } else {
        row = frame.row(r);
        timestack.push_back(row);
    }

    // cout << "ROW rows: " << row.rows << endl;
    // cout << "ROW cols: " << row.cols << endl;

    // Transposing the row
    // 

    // Pushing the transposed row to the timestack
    
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
    if (timestack.rows > timestack.cols) {
        Mat transposedTimestack;
        transpose(timestack,transposedTimestack);
        imwrite( fileName.c_str(), transposedTimestack );
    } else
        imwrite(fileName.c_str(), timestack);
}

}
