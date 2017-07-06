#ifndef __WAVEBANDDEBUGGER_H__
#define __WAVEBANDDEBUGGER_H__

#include "imageFilter.h"

#include "opencv/cv.h"

using namespace std;
using namespace cv;

namespace tcc {

class WaveBandDebugger : public ImageFilter {
private:
public:
	
	WaveBandDebugger(Mat& m) : ImageFilter() {
		filteredMat = m.clone();
	}
	
	WaveBandDebugger(Mat* m) : ImageFilter() {
		filteredMat = m->clone();
	}

	void filter() {
		if (filteredMat.channels() == 1)
			cvtColor(filteredMat,filteredMat,COLOR_GRAY2BGR);
		
		for (int i = 0; i < srcMat.cols; i++) {
			for (int j = 0; j < srcMat.rows; j++) {
				if (srcMat.at<uchar>(j,i) > 0)
					filteredMat.at<Vec3b>(j,i) = Vec3b(0,0,255);
			}
		}
	}
};

}

#endif