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
	int TOP_THRESHOLD = 0;
	for (int i = TOP_THRESHOLD; i < m.rows; i++)
		if (m.at<uchar>(i,0) > 0)
			return i;
	return 0;
}

void TiltFilter::init(Mat *m) {

	// imshow("tilt filter init:",*m);

	Mat firstCol = m->col(2);
	int firstY = findFirstPoint(firstCol);
	
	Mat lastCol = m->col(202);
	int lastY = findFirstPoint(lastCol);

	// cout << "First col: " << firstY << ", lastCol: " << lastY << endl;

	double ang = atan( (double) (lastY - firstY) / 200 ) ;

	// cout << "Ang: " << ang * 180 / PI << endl;
	// cout << "Cos: " << cos(abs(ang)) << endl;

	offsetX = m->rows * sin(abs(ang));
	offsetY = m->cols / tan(PI/2 - abs(ang));

	// cout << "OffsetX " << offsetX << endl;
	// cout << "OffsetY " << offsetY << endl;

	// cout << "X: " << offsetX << endl;
	// cout << "Y: " << offsetY << endl;

	cv::Point center(m->cols,m->rows);

	rotation = getRotationMatrix2D(center, ang * 180 / PI,1.0);
}

void TiltFilter::filter() {
	process(srcMat);
}

void TiltFilter::process(Mat &m) {
	Mat rotated;
	// cout << "rotating the image" << endl;
	warpAffine(m, rotated, rotation, m.size());
	// cout << "setting roi" << endl;
	Rect roi(offsetX,0,m.cols-2*offsetX,m.rows-2*offsetY);
	// cout << "cropping mat to rect(" << offsetX << ", " << 0 << ", " << m.cols-2*offsetX << ", " << m.rows-2*offsetY << endl;
	filteredMat = rotated(roi);
}

}