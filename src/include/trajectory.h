#ifndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

#include <vector>
#include "opencv/cv.h"

namespace tcc {

class Trajectory {
public:

	class Point {
	public:
		int x,y; 
		Point() : x(0), y(0) { }
		Point(int _x, int _y) : x(_x), y(_y) { }
	};

	std::vector<Point> points;

	Trajectory();
	Trajectory(cv::Mat&);
	~Trajectory();

	bool addPoint(int,int);
	bool isContinous(int,int);

	int calculateHeight(int,int);
	void calculateDerivative(std::vector<Point>&);

	void findFirstPoint(cv::Mat&);
	bool findNextPoint(cv::Mat&);
	bool findNextPoint(cv::Mat&, int);
	void fill(cv::Mat&);

	Point getPoint(int);
};

}

#endif
