#ifndef __SOBEL_FILTER_H__
#define __SOBEL_FILTER_H__

#include "imageFilter.h"

namespace tcc {

class SobelFilter : public ImageFilter {
public:
	SobelFilter() : ImageFilter() {

	};
	void filter();	
};

}

#endif