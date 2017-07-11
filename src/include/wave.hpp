#ifndef __WAVE_H__
#define __WAVE_H__

#include "trajectory.h"

namespace tcc {

class Wave {
public:
	Trajectory::Point bottom, top;
	Wave(Trajectory::Point b, Trajectory::Point t) : bottom(b), top(t) { }
	int getHeight() {
		return bottom.getY() - top.getY();
	}
	int getHalfway() {
		return (bottom.getX() + top.getX())/2;
	}
};

}

#endif