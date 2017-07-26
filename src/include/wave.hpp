#ifndef __WAVE_H__
#define __WAVE_H__

#include "point.hpp"

namespace tcc {

class Wave {
public:
	Point bottom, top;
	Wave(Point b, Point t) : bottom(b), top(t) { }
	int getHeight() {
		return bottom.getY() - top.getY();
	}
	int getHalfway() {
		return (bottom.getX() + top.getX())/2;
	}
};

}

#endif