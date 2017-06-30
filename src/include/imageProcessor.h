#ifndef __IMAGE_PROCESSOR_H__
#define __IMAGE_PROCESSOR_H__

#include "opencv/cv.h"
#include "imageFilter.h"

namespace tcc {


// TODO:
//	* Add getter for processed image
// 	* Add member field for processed image
class ImageProcessor {
public:
	virtual void process(cv::Mat&) = 0;
};

}

#endif