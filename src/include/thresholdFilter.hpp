#ifndef __THRESHOLD_FILTER_H__
#define __THRESHOLD_FILTER_H__

#include "opencv/cv.h"
#include "imageFilter.h"

namespace tcc {

class ThresholdFilter : public ImageFilter {
private:
	int threshold_value;
	int threshold_type;
	int binary_value;
public:
	void init() {
		threshold_value = 150;
		threshold_type = 4;
		binary_value = 0;
	}
	ThresholdFilter(int v) : ImageFilter() {
		init();
		threshold_value = v;
	};
	ThresholdFilter(int v, int t, int bv) : ImageFilter() {
		threshold_value = v;
		threshold_type = t;
		binary_value = bv;
	};
	ThresholdFilter() : ImageFilter() {
		init();
	};
	void filter() {
		// int threshold_value = 150;
		threshold( srcMat, filteredMat, threshold_value, binary_value,threshold_type );
	};
};

}

#endif