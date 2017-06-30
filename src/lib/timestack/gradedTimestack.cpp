#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "gradedTimestack.h"

namespace tcc {

using namespace cv;
using namespace std;

GradedTimestack::GradedTimestack(VideoCapture& cap) : Timestack(cap) {
	fps = (int) cap.get(CV_CAP_PROP_FPS);
}

void GradedTimestack::save(std::string fileName) {
	Mat transposedTimestack;

	Mat* timestack = this->getTimestack();
    transpose(*timestack,transposedTimestack);

    Mat coloredTimestack;
    cvtColor(transposedTimestack, coloredTimestack, CV_GRAY2RGB);

    int width = coloredTimestack.size().width;
    int height = coloredTimestack.size().height;
    for(int i=0;i<width;i+=fps)
  		cv::line(coloredTimestack,Point(i,0),Point(i,height),cv::Scalar(255,255,255));

    imwrite( "graded_" + fileName, coloredTimestack );
}

}