#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv/cv.h"

#include "laplacianFilter.h"

namespace tcc {

using namespace std;
using namespace cv;

void LaplacianFilter::filter() {
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Mat aux,laplacian;

	Laplacian( srcMat, laplacian, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );

	convertScaleAbs( laplacian, aux );

	filteredMat = srcMat + aux;
}

}