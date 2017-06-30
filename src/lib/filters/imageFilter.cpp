#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv/cv.h"
#include <opencv2/highgui/highgui.hpp>

#include "imageFilter.h"

namespace tcc {

using namespace std;
using namespace cv;

ImageFilter::ImageFilter() {

}

ImageFilter::ImageFilter(Mat& mat) {
	srcMat = mat.clone();
}

ImageFilter::ImageFilter(Mat* mat) {
	setSourceMat(mat);
}

void ImageFilter::setSourceMat(Mat* mat) {
	srcMat = mat->clone();
}

Mat* ImageFilter::getFilteredImage() {
	return &filteredMat;
}

Mat* ImageFilter::getSourceMat() {
	return &srcMat;
}

void ImageFilter::save(std::string fileName) {
	imwrite(fileName,filteredMat);
}

}