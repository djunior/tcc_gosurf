#ifndef __LAPLACIAN_FILTER_H__
#define __LAPLACIAN_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class LaplacianFilter: public ImageFilter {
public:
	LaplacianFilter() : ImageFilter() {};
	LaplacianFilter(cv::Mat& m) : ImageFilter(m) {	};
	void filter();
};

}

#endif