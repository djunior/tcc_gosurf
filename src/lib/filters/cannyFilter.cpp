#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

#include "cannyFilter.h"

using namespace cv;

namespace tcc {

void CannyFilter::setLowThreshold(int t) {
	lowThreshold = t;
}

void CannyFilter::filter() {
	Canny( srcMat, filteredMat, lowThreshold, lowThreshold*ratio, kernel_size );
}

}