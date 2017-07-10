#ifndef __WAVEBANDDEBUGGER_H__
#define __WAVEBANDDEBUGGER_H__

#include "imageFilter.h"
#include "trajectory.h"
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

		Mat debug = filteredMat.clone();
		
		for (int i = 0; i < srcMat.cols; i++) {
			for (int j = 0; j < srcMat.rows; j++) {
				if (srcMat.at<uchar>(j,i) > 0)
					filteredMat.at<Vec3b>(j,i) = Vec3b(0,0,255);
			}
		}

		cout << "Creating trajectory" << endl;
		Trajectory t(srcMat);

		cout << "populating debug(" << debug.cols << "," << debug.rows << ")" << endl;
		for (int i = 0; i < t.points.size(); i++) {
			// cout << "Point: " << t.points[i].y << ", " << t.points[i].x << endl;
			debug.at<Vec3b>(t.points[i].x,t.points[i].y) = Vec3b(0,0,255);
		}

		cout << "resizing video" << endl;
		// resize(debug,debug,Size(debug.cols/2,debug.rows/2));

		cout << "showing video" << endl;
		imshow("trajectory debug",debug);

	}
};

}

#endif