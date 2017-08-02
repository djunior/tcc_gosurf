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

void TiltFilter::init(Mat *m) {
	setSourceMat(m);

	Mat firstCol = srcMat.col(0);
	int firstY = findFirstPoint(firstCol);
	
	Mat lastCol = srcMat.col(srcMat.cols-1);
	int lastY = findFirstPoint(lastCol);

	double ang = atan( (double) (lastY - firstY) / (srcMat.cols) ) ;

	// cout << "Ang: " << ang * 180 / PI << endl;
	// cout << "Cos: " << cos(abs(ang)) << endl;

	offsetX = srcMat.rows * sin(abs(ang));
	offsetY = srcMat.cols / tan(PI/2 - abs(ang));

	// cout << "X: " << x << endl;
	// cout << "Y: " << y << endl;

	cv::Point center(srcMat.cols,srcMat.rows);

	rotation = getRotationMatrix2D(center, ang * 180 / PI,1.0);
}

void TiltFilter::filter() {

}

void TiltFilter::process(Mat &m) {
	Mat rotated;
	warpAffine(m, rotated, rotation, m.size());
	Rect roi(offsetX,0,m.cols-2*offsetX,m.rows-2*offsetY);
	filteredMat = rotated(roi);
}

}