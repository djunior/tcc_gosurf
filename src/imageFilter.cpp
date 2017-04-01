#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv/cv.h"
#include <opencv2/highgui/highgui.hpp>
#include "imageFilter.h"

namespace tcc {

using namespace std;
using namespace cv;

ImageFilter::ImageFilter(Mat& mat) {
	srcMat = mat.clone();
}

void ImageFilter::filter() {
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Mat aux, auxGray, dst;

	/// Remove noise by blurring with a Gaussian filter
	GaussianBlur( srcMat, aux, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Convert the image to grayscale
	cvtColor( aux, auxGray, CV_BGR2GRAY );

	Laplacian( auxGray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( dst, filteredMat );
}

Mat* ImageFilter::getFilteredImage() {
	return &filteredMat;
}

void ImageFilter::save(std::string fileName) {
	imwrite(fileName,filteredMat);
}

}