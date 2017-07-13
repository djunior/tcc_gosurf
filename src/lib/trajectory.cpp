#include "trajectory.h"

#include <iostream>

using namespace std;
using namespace cv;

namespace tcc {

Trajectory::Trajectory() {

}

Trajectory::Trajectory(cv::Mat& m) {
	findFirstPoint(m);
	fill(m);
}

Trajectory::Trajectory(cv::Mat& m, cv::Rect area) {
	findFirstPoint(m,area);
	fill(m,area);
}

Trajectory::~Trajectory() {
	points.clear();
}

bool Trajectory::addPoint(int x, int y) {
	Trajectory::Point p;
	p.x = x;
	p.y = y;

	for (int i = 0; i < points.size(); i++)
		if (points[i].x == x && points[i].y == y)
			return false;

	points.push_back(p);

	return true;
};

int abs(int n) {
	if (n < 0)
		return -1*n;
	return n;
}

bool Trajectory::isContinous(int x, int y) {
	if (points.size() == 0)
		return true;

	Point p = points.back();

	int dX = abs(p.x - x);
	int dY = abs(p.y - y);

	if (dX > 1 || dY > 1)
		return false;

	return true;
}

int Trajectory::calculateHeight(int bottom, int top) {
	return points[bottom].getY() - points[top].getY();
}

void Trajectory::calculateDerivative(std::vector<Point>& d) {
	for (int i = 0; i < points.size(); i++) {
	
		int x = points[i].getX();
		int y = points[i].getY();

		if (i > 0) {
			int dX = x - points[i-1].getX();
			int dY = y - points[i-1].getY();
			Point p(dX, dY);
			d.push_back(p);
		}
	}
}

void Trajectory::fill(Mat& m) {
	fill(m,Rect(0,0,m.cols,m.rows));
}

void Trajectory::fill(Mat& m, Rect roi) {
	while(true)
		if (! findNextPoint(m,roi))
			break;
}

bool Trajectory::findNextPoint(Mat& m) {
	return findNextPoint(m,1,Rect(0,0,m.cols,m.rows));
}

bool Trajectory::findNextPoint(Mat& m,Rect roi) {
	return findNextPoint(m,1,roi);
}

bool Trajectory::findNextPoint(Mat& m, int threshold, Rect roi) {
	Trajectory::Point currentPoint = points.back();

	if (currentPoint.getX() >= (roi.x + roi.width - 2))
		return false;

	int MAX_THRESHOLD = 3;

	int beginY = currentPoint.getY() - threshold > (roi.y + 2) ? currentPoint.getY() - threshold : roi.y + 2;
	int endY = currentPoint.getY() + threshold < (roi.y + roi.height - 2) ? currentPoint.getY() + threshold : roi.y + roi.height - 2;

	int beginX = currentPoint.getX() - threshold > (roi.x + 2) ? currentPoint.getX() - threshold : roi.x + 2;
	int endX = currentPoint.getX() + threshold < (roi.x + roi.width - 2) ? currentPoint.getX() + threshold : roi.x + roi.width - 2;

	for (int i = beginY; i <= endY; i++)
		for (int j = beginX; j <= endX; j++)
			if (m.at<uchar>(i,j) > 0 && addPoint(j,i))
				return true;

	if (threshold < MAX_THRESHOLD)
		return findNextPoint(m,threshold+1,roi);

	return false;
}

void Trajectory::findFirstPoint(Mat& m) {
	findFirstPoint(m,Rect(0,0,m.cols,m.rows));
}

void Trajectory::findFirstPoint(Mat& m, cv::Rect roi) {
	int width = m.cols;
	int height = m.rows;

	for (int i = (1+roi.x); i < (roi.x + roi.width - 1); i++) {
		Mat col = m.col(i);

		bool found = false;
		for (int j = (2+roi.y); j < (roi.y+roi.height - 1); j++) {
			float v = (float) col.at<uchar>(j) / 255;
			
			if (v > 0) {
				found = true;
				addPoint(i,j);
				break;
			}
		}

		if (found)
			break;

	}	
}

Trajectory::Point Trajectory::getPoint(int i) {
	return points[i];
}

}