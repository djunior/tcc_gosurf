#ifndef __IMAGE_FILTER_H__
#define __IMAGE_FILTER_H__

namespace tcc {

class ImageFilter {
public:
	ImageFilter(cv::Mat&);
	void filter();
	void save(std::string);
	cv::Mat* getFilteredImage();
private:
	cv::Mat srcMat,filteredMat;
};

}

#endif