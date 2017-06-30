#ifndef __IMAGE_VIEWER_HPP__
#define __IMAGE_VIEWER_HPP__

#include "imageFilter.h"
#include "opencv/cv.h"

namespace tcc {

using namespace cv;

class ImageViewer : public ImageFilter {
private:
	std::string window_name;
public:
	ImageViewer() : ImageFilter() {
		init();
	}
	ImageViewer(Mat& m) : ImageFilter(m) { 
		init(); 
	};
	ImageViewer(std::string name) : ImageFilter() {
		setWindowName(name);
	}
	void init() {
		window_name = "image viewer";
	};
	void setWindowName(std::string name) {
		window_name = name;
	};
	void filter() {
		filteredMat = srcMat.clone();
		resize(srcMat, srcMat, Size(srcMat.cols/2, srcMat.rows/2));
		imshow(window_name,srcMat);
	};
};

}

#endif