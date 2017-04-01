#ifndef __TIMESTACK_H__
#define __TIMESTACK_H__

#define NUM_FRAMES_TO_SKIP 1
#define CAPTURE_DURATION_SEC 

namespace tcc {

class Timestack {
private:
	cv::Mat timestack;
	int size;

public:
	Timestack(cv::VideoCapture&);

	void addFrame(cv::Mat &);

	cv::Mat* getTimestack();

	void calculateTimestack(std::string);

	void save(std::string fileName);
};

}
#endif