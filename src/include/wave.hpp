#ifndef __WAVE_H__
#define __WAVE_H__

#include "trajectory.h"

namespace tcc {

class Wave {
public:
	Trajectory::Point bottom, top;
	Wave(Trajectory::Point b, Trajectory::Point t) : bottom(b), top(t) { }
};

}

#endif