#ifndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

#include <vector>
#include "opencv/cv.h"

namespace tcc {

class Trajectory {
private:
	bool findNextPoint(cv::Mat&, int, cv::Rect);
public:

	class Point {
	public:
		int x,y; 
		Point() : x(0), y(0) { }
		Point(int _x, int _y) : x(_x), y(_y) { }
		int getX() {
			return x;
		}
		int getY() {
			return y;
		}
	};

	std::vector<Point> points;

	Trajectory();
	Trajectory(cv::Mat&);
	Trajectory(cv::Mat&, cv::Rect);
	~Trajectory();

	bool addPoint(int,int);
	bool isContinous(int,int);

	int calculateHeight(int,int);
	void calculateDerivative(std::vector<Point>&);

	void findFirstPoint(cv::Mat&);
	void findFirstPoint(cv::Mat&, cv::Rect);
	
	bool findNextPoint(cv::Mat&);
	bool findNextPoint(cv::Mat&, cv::Rect);

	void fill(cv::Mat&);
	void fill(cv::Mat&, cv::Rect);

	Point getPoint(int);
};

}

#endif
