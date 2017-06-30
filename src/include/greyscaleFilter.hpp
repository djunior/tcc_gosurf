#ifndef __GREYSCALE_FILTER_H__
#define __GREYSCALE_FILTER_H__

#include "imageFilter.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

namespace tcc {

using namespace cv;

class GreyscaleFilter : public ImageFilter {
public:
	void filter() {
		cvtColor( srcMat, filteredMat, CV_BGR2GRAY );
	};
};

}


#endif