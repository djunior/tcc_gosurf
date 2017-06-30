#ifndef __SKY_REMOVER_FILTER_H__
#define __SKY_REMOVER_FILTER_H__

#include "imageFilter.h"
#include "opencv/cv.h"

using namespace cv;

namespace tcc {

class SkyRemoverFilter : public ImageFilter {
private:
	int findFirstPoint(Mat &m) {
		int TOP_THRESHOLD = 300;
		for (int i = TOP_THRESHOLD; i < m.rows; i++)
			if (m.at<uchar>(i,0) > 0)
				return i;
		return 0;
	}

	Mat* originalImage;
public:

	SkyRemoverFilter(Mat* m) { originalImage = m; }

	void filter() {
		Mat mask(srcMat.size(),CV_8UC1,Scalar::all(255));

		int TOP_THRESHOLD = 300;

		for (int i = 0; i < srcMat.cols; i++) {
			for (int j = 0; j < srcMat.rows; j++) {
				mask.at<uchar>(j,i) = 0;
				if (j > TOP_THRESHOLD && srcMat.at<uchar>(j,i) > 0)
					break;
			}
		}

		originalImage->copyTo(filteredMat,mask);
	}
};

}

#endif
