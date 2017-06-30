#ifndef __CANNY_FILTER_H__
#define __CANNY_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class CannyFilter: public ImageFilter {
public:
	CannyFilter() {
		init();
	};
	CannyFilter(cv::Mat& m) : ImageFilter(m) {
		init();
	};
	CannyFilter(int t) : ImageFilter() {
		init();
		setLowThreshold(t);
	}
	void init() {
		ratio = 3;
		kernel_size = 3;
		lowThreshold = 0;
	};
	void setLowThreshold(int t);
	void filter();
private:
	int ratio;
	int kernel_size;
	int lowThreshold;
};

}


#endif