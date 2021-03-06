#ifndef __SKY_REMOVER_FILTER_H__
#define __SKY_REMOVER_FILTER_H__

#include "imageFilter.h"
#include "opencv/cv.h"
#include "trajectory.h"

#include "imageProcessor.h"

using namespace cv;

namespace tcc {

// TODO: change this to be an implementation of ImageProcessor only.
class SkyRemoverFilter : public ImageFilter, public ImageProcessor {
private:
	int findFirstPoint(Mat &m) {
		int TOP_THRESHOLD = 0;
		for (int i = TOP_THRESHOLD; i < m.rows; i++)
			if (m.at<uchar>(i,0) > 0)
				return i;
		return 0;
	}

	Mat mask;
public:

	SkyRemoverFilter() : ImageFilter() {

	}

	SkyRemoverFilter(Mat &m) : ImageFilter() { 
		init(m);
	}

	void init(Mat& m) {
		int y = findFirstPoint(m);

		Trajectory t;
		t.addPoint(0,y);
		t.trackLine(m);
		mask = Mat::ones(m.size(),CV_8UC1);

		for (int i = 0; i < t.points.size(); i++) {
			tcc::Point p = t.getPoint(i);
			for (int j = 0; j < p.getY(); j++) {
				mask.at<uchar>(j,i) = 0;
			}
		}

		imshow("output_images/simpletimestack/simpletimestack_sky_remover_mask.jpg",mask);
	}

	void filter() {

		// Trajectory t;

		// for (int i = 0; i < srcMat.cols; i++) {
		// 	for (int j = 0; j < srcMat.rows; j++) {
		// 		mask.at<uchar>(j,i) = 0;
		// 		if (j > TOP_THRESHOLD && srcMat.at<uchar>(j,i) > 0)
		// 			break;
		// 	}
		// }

		srcMat.copyTo(filteredMat,mask);
	}

	void process(Mat& m) {
		m.copyTo(filteredMat, mask);
	}
};

}

#endif
