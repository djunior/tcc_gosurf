#ifndef __TIMESTACK_H__
#define __TIMESTACK_H__

#define NUM_FRAMES_TO_SKIP 1
#define CAPTURE_DURATION_SEC 

#include "opencv/cv.h"
#include "imageProcessor.h"

namespace tcc {

class Timestack : public ImageProcessor {
private:
	cv::Mat timestack;
	int size;
	int horizontalPercent;

public:
	Timestack(cv::VideoCapture&);

	void process(cv::Mat &);

	cv::Mat* getTimestack();

	void calculateTimestack(std::string);

	void setHorizontalPercent(int);
	int getHorizontalPercent();

	void save(std::string fileName);
};

}
#endif