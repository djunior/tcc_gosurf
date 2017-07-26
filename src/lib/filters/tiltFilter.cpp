#include "tiltFilter.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv/cv.h"

#include "trajectory.h"

#include <cmath>

#define PI 3.14159265

using namespace std;
using namespace cv;

namespace tcc {

int TiltFilter::findFirstPoint(Mat &m) {
	int TOP_THRESHOLD = 300;
	for (int i = TOP_THRESHOLD; i < m.rows; i++)
		if (m.at<uchar>(i,0) > 0)
			return i;
	return 0;
}

void TiltFilter::filter() {

	Mat firstCol = srcMat.col(0);
	int firstY = findFirstPoint(firstCol);
	
	Mat lastCol = srcMat.col(srcMat.cols-1);
	int lastY = findFirstPoint(lastCol);

	double ang = -1* atan( (double) abs(lastY - firstY) / (srcMat.cols-1) ) * 180 / PI;

	cv::Point center(srcMat.cols,srcMat.rows);

	Mat r = getRotationMatrix2D(center,ang,1.0);

	warpAffine(srcMat, filteredMat, r, srcMat.size());
}

}