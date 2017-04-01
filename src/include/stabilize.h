#ifndef __STABILIZE_H__
#define __STABILIZE_H__
#include "opencv/cv.h"
#include "timestack.h"

namespace tcc {

void stabilize(cv::VideoCapture &cap, Timestack& ot, Timestack& st);

}

#endif