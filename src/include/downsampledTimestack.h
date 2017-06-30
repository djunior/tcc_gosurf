#ifndef __DOWNSAMPLED_TIMESTACK_H__
#define __DOWNSAMPLED_TIMESTACK_H__

#include "opencv/cv.h"
#include "timestack.h"

namespace tcc {

class DownsampledTimestack : public Timestack {
private:
	int frameCount, sampleRate, downsampleFactor, originalSampleRate;
public:
	DownsampledTimestack(cv::VideoCapture&,int);
	
	void setSampleRate(int);
	int getSampleRate();

	void setDownsampleFactor(int);
	int getDownsampleFactor();
	void process(cv::Mat&);
};

}


#endif