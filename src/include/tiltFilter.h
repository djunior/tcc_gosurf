#ifndef __TILT_FILTER_H__
#define __TILT_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class TiltFilter : public ImageFilter {
public:
	TiltFilter() : ImageFilter() {}
	void filter();
private:
	int findFirstPoint(cv::Mat &);
};


}

#endif