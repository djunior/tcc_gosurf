#include "histogramEqualizatorFilter.h"
#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

namespace tcc {

void HistogramEqualizatorFilter::filter() {
	equalizeHist( srcMat, filteredMat );
}

}