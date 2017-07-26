#ifndef __DERIVABLE_H__
#define __DERIVABLE_H__

#include "point.hpp"

namespace tcc {

class Derivable {
public:
	std::vector<Point> points;

	bool addPoint(int x, int y) {
		Point p(x,y);

		points.push_back(p);

		return true;
	}
	
	bool isContinous(int x,int y) {
		if (points.size() == 0)
			return true;

		Point p = points.back();

		int dX = abs(p.getX() - x);
		int dY = abs(p.getY() - y);

		if (dX > 1 || dY > 1)
			return false;

		return true;
	}


	int calculateHeight(int bottom, int top) {
		return points[bottom].getY() - points[top].getY();
	}

	void calculateDerivative(std::vector<Point>& p) {
		Derivable d;
		calculateDerivative(d);
		for (int i = 0; i < d.points.size(); i++) {
			p.push_back(d.points[i]);
		}
	}

	void calculateDerivative(Derivable& d) {
		for (int i = 0; i < points.size(); i++) {
			int x = points[i].getX();
			int y = points[i].getY();

			if (i > 0) {
				int dX = x - points[i-1].getX();
				int dY = y - points[i-1].getY();
				d.addPoint(dX,dY);
			}
		}
	}

	Point getPoint(int i) {
		return points[i];
	}
};

}

#endif