#ifndef __WAVE_DETECTOR_H__
#define __WAVE_DETECTOR_H__

#include "trajectory.h"
#include "opencv/cv.h"
#include "imageFilter.h"

namespace tcc {

class WaveDetector : public ImageFilter {
public:
	std::vector<Trajectory::Point> waves;
	~WaveDetector();
	void filter();
	void drawWaves(cv::Mat&);
private:
	std::vector<Trajectory*> trajectoryList;
	void detectWave(Trajectory&,int,int);
	void fillTrajectory(Trajectory&);
	void findFirstPoint(Trajectory&);
	bool findNextPoint(Trajectory&);
};


}


#endif