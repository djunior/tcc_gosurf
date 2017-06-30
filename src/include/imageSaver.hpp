#ifndef __IMAGE_SAVER_H__
#define __IMAGE_SAVER_H__

#include <iostream>
#include <opencv2/core/core.hpp>
#include "imageFilter.h"

namespace tcc {

class ImageSaver : public ImageFilter {
private:
	std::string filename;
public:
	ImageSaver() : ImageFilter() { init(); };
	ImageSaver(std::string fn) : ImageFilter() { filename = fn; };
	ImageSaver(cv::Mat& m) : ImageFilter(m) { init(); };
	void init() { filename = ""; };
	void filter() {
		filteredMat = srcMat.clone();
		save(filename);
	};
};

}

#endif