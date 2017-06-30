#ifndef __STABILIZE_H__
#define __STABILIZE_H__
#include "opencv/cv.h"
#include "imageProcessor.h"

namespace tcc {

void stabilize(cv::VideoCapture&);
void stabilize(cv::VideoCapture&, ImageProcessor*, ImageProcessor*);

}

#endif