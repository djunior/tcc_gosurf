#ifndef __COLORED_POINT_H__
#define __COLORED_POINT_H__

#include "opencv/cv.h"

#include "point.hpp"

namespace tcc {

class ColoredPoint : public Point {
public:
	cv::Scalar color;

	ColoredPoint() : Point() {}

	ColoredPoint(cv::Scalar c) : Point() {
		color = c;
	}

	ColoredPoint(int x, int y, cv::Scalar c) : Point(x,y) {
		color = c;
	}
};
	
}

#endif