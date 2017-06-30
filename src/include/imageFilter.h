#ifndef __IMAGE_FILTER_H__
#define __IMAGE_FILTER_H__

#include "imageProcessor.h"

namespace tcc {

// TODO: 
// 	* Make ImageFilter extend ImageProcessor
class ImageFilter {
public:
	ImageFilter();
	ImageFilter(cv::Mat&);
	ImageFilter(cv::Mat*);
	virtual void filter() = 0;
	void save(std::string);
	cv::Mat* getFilteredImage();
	void setSourceMat(cv::Mat* mat);
	cv::Mat* getSourceMat();
protected:
	cv::Mat srcMat,filteredMat;
};

}

#endif