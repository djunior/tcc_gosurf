#ifndef __POINT_H__
#define __POINT_H__

namespace tcc {

class Point {
private:
	int x,y;
public:
	Point() : x(0), y(0) { }
	Point(int _x, int _y) : x(_x), y(_y) { }
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
};

}

#endif