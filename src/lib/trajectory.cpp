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

Trajectory::~Trajectory() {
	points.clear();
}

bool Trajectory::addPoint(int x, int y) {
	// cout << "Trajectory::addPoint Checking if point is continous in trajectory" << endl;
	// if (points.size() > 0 && ! isContinous(x,y))
	// 	return false;

	Trajectory::Point p;
	p.x = x;
	p.y = y;

	// cout << "Trajectory::addPoint left = " << left << endl;
	// cout << "Trajectory::addPoint right = " << right << endl;
	// cout << "Trajectory::addPoint top = " << top << endl;
	// cout << "Trajectory::addPoint bottom = " << bottom << endl;

	for (int i = 0; i < points.size(); i++)
		if (points[i].x == x && points[i].y == y)
			return false;

	points.push_back(p);

	// if (!left || x < left->x){
	// 	cout << "Trajectory::addPoint updating most left point" << endl;
	// 	left = &points.back();
	// }

	// if (!right || x > right->x) {
	// 	cout << "Trajectory::addPoint updating most right point" << endl;
	// 	right = &points.back();
	// }			
		
	// if (!bottom || y < bottom->y) {
	// 	cout << "Trajectory::addPoint updating most bottom point" << endl;
	// 	bottom = &points.back();
	// }

	// if (!top || y > top->y) {
	// 	cout << "Trajectory::addPoint updating most top point" << endl;
	// 	top = &points.back();
	// }

	// cout << "Trajectory::addPoint done, returning true" << endl;

	return true;
};

// Point* getLeft() {
// 	return left;
// }

// Point* getRight() {
// 	return right;
// }

// Point* getTop() {
// 	return top;
// }

// Point* getBottom() {
// 	return bottom;
// }

int abs(int n) {
	if (n < 0)
		return -1*n;
	return n;
}

bool Trajectory::isContinous(int x, int y) {
	// cout << "Trajectory::isContinous (" << x << "," << y << ")" << endl;
	if (points.size() == 0)
		return true;

	Point p = points.back();

	// cout << "Trajectory::isContinous lastPoint: ";
	// cout << p;
	// cout << endl;

	int dX = abs(p.x - x);
	int dY = abs(p.y - y);

	// std::cout << "dX " << dX << std::endl;
	// std::cout << "dY " << dY << std::endl;

	if (dX > 1 || dY > 1) {
		// cout << "Trajectory::isContinous returning false" << endl;
		return false;
	}

	// cout << "Trajectory::isContinous returning true" << endl;
	return true;
}

int Trajectory::calculateHeight(int bottom, int top) {
	int height = points[bottom].x - points[top].x;
	// cout << "Calculating height from " << bottom << " to " << top << ": " << height << endl;
	return height;
}

void Trajectory::calculateDerivative(std::vector<Point>& d) {
	// Point bottom(-1,-1);
	// Point top(-1,-1);

	cout << "Points size: " << points.size() << endl;

	for (int i = 0; i < points.size(); i++) {
	
		int x = points[i].x;
		int y = points[i].y;

		// if (bottom.x == -1 || x >= bottom.x) {
		// 	bottom.x = x;
		// 	bottom.y = y;
		// }

		// if (top.x == -1 || x <= top.x) {
		// 	top.x = x;
		// 	top.y = y;
		// }

		if (i > 0) {
			int dX = x - points[i-1].x;
			int dY = y - points[i-1].y;
			Point p(dX, dY);
			d.push_back(p);
		}
	}
}

void Trajectory::fill(Mat& m) {
	while(1) {

		if (! findNextPoint(m)) {
			cout << "Finished trajectory" << endl;
			break;
		}

	}
}

bool Trajectory::findNextPoint(Mat& m) {
	int THRESHOLD = 1;
	return findNextPoint(m,THRESHOLD);
}

bool Trajectory::findNextPoint(Mat& m, int threshold) {

	Trajectory::Point currentPoint = points.back();

	int MAX_THRESHOLD = 3;

	// cout << "Trajectory::findNextPoint(m," << threshold << ")" << endl;

	// cout << "current point: (" << currentPoint.x << "," << currentPoint.y << ")" << endl;

	int beginX = currentPoint.x > (threshold+2) ? currentPoint.x - threshold : 2 + threshold;
	int endX = currentPoint.x < m.rows - 2 - threshold ? currentPoint.x + threshold : m.rows - 2 - threshold;

	int endY = currentPoint.y > (threshold+2) ? currentPoint.y + threshold : 2 + threshold;
	int beginY = currentPoint.y < m.cols - 2 - threshold ? currentPoint.y - threshold : m.cols - 2 - threshold;

	// cout << "Looking for point from (" << beginX << "," << beginY << ") to (" << endX << "," << endY << ")" << endl;

	for (int i = beginX; i <= endX; i++) {
		for (int j = beginY; j <= endY; j++) {

			// cout << "Pixel(" << i << "," << j << ") = " << (int) m.at<uchar>(i,j) << endl;

			if (m.at<uchar>(i,j) > 0) {
				if (addPoint(i,j))
					return true;
			}
		}
	}

	if (threshold < MAX_THRESHOLD)
		return findNextPoint(m,threshold+1);


	cout << "Could not find next point for current point (" << currentPoint.x << "," << currentPoint.y << ")" << endl;

	return false;

}

void Trajectory::findFirstPoint(Mat& m) {
	int width = m.cols;
	int height = m.rows;

	for (int j = 1; j < (width-1); j++) {
		Mat col = m.col(j);

		bool found = false;
		for (int i = 2; i < (col.rows-1); i++) {
			float v = (float) col.at<uchar>(i) / 255;
			
			if (v > 0) {
				cout << "Found first point at: (" << i << "," << j << ")" << endl;
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