#ifndef __WAVE_DETECTOR_H__
#define __WAVE_DETECTOR_H__

#include "trajectory.h"
#include "opencv/cv.h"
#include "imageFilter.h"
#include "wave.hpp"

namespace tcc {

class WaveDetector : public ImageFilter {
public:
	std::vector<Wave> waves;
	~WaveDetector();
	void filter();
	void drawWaves(cv::Mat&);
	void save(char*);
private:
	std::vector<Trajectory*> trajectoryList;
	void detectWave(Trajectory&,int,int);
	void fillTrajectory(Trajectory&);
	void findFirstPoint(Trajectory&);
	bool findNextPoint(Trajectory&);
	void analyseTrajectory(Trajectory&);
};


}


#endif