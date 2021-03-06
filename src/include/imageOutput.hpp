#ifndef __IMAGE_OUTPUT_H__
#define __IMAGE_OUTPUT_H__

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "imageFilter.h"

namespace tcc {

class ImageOutput : public ImageFilter {
private:
	std::string filename;
public:
	ImageOutput() : ImageFilter() { init(); };
	ImageOutput(std::string fn) : ImageFilter() { filename = fn; };
	ImageOutput(cv::Mat& m) : ImageFilter(m) { init(); };
	void init() { filename = ""; };
	void filter() {
		filteredMat = srcMat.clone();
		char* mode = getenv ("DEBUG");
		if (mode && strcmp(mode,"n") == 0) {
			save(filename);
		} else {
			cv::Mat debug;
			cv::resize(srcMat, debug, cv::Size(srcMat.cols/2, srcMat.rows/2));
			cv::imshow(filename,debug);
		}
	};
};

}

#endif