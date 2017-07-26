#ifndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

#include <vector>
#include "opencv/cv.h"

#include "derivable.hpp"

namespace tcc {

class Trajectory : public Derivable {
private:
	bool findNextPoint(cv::Mat&);
	bool findNextPoint(cv::Mat&, int, cv::Rect);
	bool findNextPoint(cv::Mat&, cv::Rect);
public:

	Trajectory();
	Trajectory(cv::Mat&);
	Trajectory(cv::Mat&, cv::Rect);

	bool addPoint(int,int);

	void findFirstPoint(cv::Mat&);
	void findFirstPoint(cv::Mat&, cv::Rect);
	
	void trackLine(cv::Mat&);
	void trackLine(cv::Mat&, cv::Rect);
};

}

#endif
