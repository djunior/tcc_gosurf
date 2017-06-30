#ifndef __GAUSSIAN_BLUR_FILTER_H__
#define __GAUSSIAN_BLUR_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class GaussianBlurFilter: public ImageFilter {
public:
	GaussianBlurFilter() : ImageFilter() { init(); };
	GaussianBlurFilter(cv::Mat& m) : ImageFilter(m) {
		init();
	};
	GaussianBlurFilter(int ks) : ImageFilter() {
		init();
		setKernelSize(ks);
	};
	void init() { kernel_size = 3; }
	void setKernelSize(int);
	void filter();
private:
	int kernel_size;
};

}


#endif