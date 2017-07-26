#include "trajectory.h"

#include <iostream>

using namespace std;
using namespace cv;

namespace tcc {

Trajectory::Trajectory() {

}

Trajectory::Trajectory(cv::Mat& m) {
	findFirstPoint(m);
	trackLine(m);
}

Trajectory::Trajectory(cv::Mat& m, cv::Rect area) {
	findFirstPoint(m,area);
	trackLine(m,area);
}


bool Trajectory::addPoint(int x, int y) {
	tcc::Point p(x,y);

	for (int i = 0; i < points.size(); i++)
		if (points[i].getX() == x && points[i].getY() == y)
			return false;

	points.push_back(p);

	return true;
};

int abs(int n) {
	if (n < 0)
		return -1*n;
	return n;
}

void Trajectory::trackLine(Mat& m) {
	trackLine(m,Rect(0,0,m.cols,m.rows));
}

void Trajectory::trackLine(Mat& m, Rect roi) {
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

bool Trajectory::findNextPoint(Mat& m, int radius, Rect roi) {
	tcc::Point currentPoint = points.back();

	if (currentPoint.getX() >= (roi.x + roi.width - 2))
		return false;

	int MAX_RADIUS = 3;

	int beginY = currentPoint.getY() - radius > (roi.y + 2) ? currentPoint.getY() - radius : roi.y + 2;
	int endY = currentPoint.getY() + radius < (roi.y + roi.height - 2) ? currentPoint.getY() + radius : roi.y + roi.height - 2;

	int beginX = currentPoint.getX() - radius > (roi.x + 2) ? currentPoint.getX() - radius : roi.x + 2;
	int endX = currentPoint.getX() + radius < (roi.x + roi.width - 2) ? currentPoint.getX() + radius : roi.x + roi.width - 2;

	for (int i = beginY; i <= endY; i++)
		for (int j = beginX; j <= endX; j++)
			if (m.at<uchar>(i,j) > 0 && addPoint(j,i))
				return true;

	if (radius < MAX_RADIUS)
		return findNextPoint(m,radius+1,roi);

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

}