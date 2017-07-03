#include "sobelFilter.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv/cv.h"

using namespace std;
using namespace cv;

namespace tcc {

void SobelFilter::filter() {
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Mat sobelFiltered;
	Mat sobelY;
	Mat sobelX;

	Sobel(srcMat, sobelFiltered, ddepth, 1, 0, kernel_size, scale, delta, BORDER_DEFAULT );
	// Sobel(srcMat, sobelX, ddepth, 1, 0, kernel_size, scale, delta, BORDER_DEFAULT );

	// Laplacian( srcMat, sobelFiltered, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );

	// sobelFiltered = sobelX + sobelY;

	convertScaleAbs( sobelFiltered, filteredMat );

	// filteredMat = srcMat + sobelFiltered;
}

}