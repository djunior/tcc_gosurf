#ifndef __WAVE_LIST_HPP__
#define __WAVE_LIST_HPP__

#include <iostream>
#include <fstream>
#include "wave.hpp"
#include <vector>

using namespace std;

namespace tcc {


class WaveList {
private:
	vector<Wave> waves;
public:
	WaveList() {

	}
	
	void addWave(Wave wave) {
		waves.push_back(wave);
	}

	void addWave(Point bottom, Point top) {
		waves.push_back(Wave(bottom,top));
	}

	Wave getWave(int index) {
		return waves[index];
	}

	int getSize() {
		return waves.size();
	}

	void load(string fileName) {

		string line_bottom;
		string line_top;
		
		ifstream f(fileName.c_str());

		if (f.is_open())
		{
			while ( f >> line_bottom >> line_top)
		    {
				int bottom_x,bottom_y;
				int top_x,top_y;

				sscanf(line_bottom.c_str(),"%i,%i",&bottom_x,&bottom_y);
				sscanf(line_top.c_str(),"%i,%i",&top_x,&top_y);

				tcc::Point bottom(bottom_x,bottom_y);
				tcc::Point top(top_x,top_y);

				addWave(bottom,top);
		    }
		    f.close();
		}

	}

	void save(string fileName, string info) {
		ofstream f(fileName.c_str());

		if (f.is_open())
		{
			// f << "# " << info << endl;
			for (int i = 0; i < getSize(); i ++) {
		  		tcc::Point bottom = getWave(i).bottom;
		  		tcc::Point top = getWave(i).top;

		  		f << bottom.getX() << "," << bottom.getY() << endl;
		  		f << top.getX() << "," << top.getY() << endl;
		  	}
		    f.close();
		}
	}

};
	
}


#endif