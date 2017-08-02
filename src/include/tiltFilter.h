#ifndef __TILT_FILTER_H__
#define __TILT_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class TiltFilter : public ImageFilter, public ImageProcessor {
public:
	TiltFilter() : ImageFilter() {}
	void init(cv::Mat*);
	void filter();
	void process(cv::Mat &);
private:
	cv::Mat rotation;
	int offsetX, offsetY;
	int findFirstPoint(cv::Mat &);
};


}

#endif