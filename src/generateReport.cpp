#include <iostream>
#include <fstream>
#include "wave.hpp"
#include "waveList.hpp"
#include "camera.hpp"

using namespace std;
using namespace tcc;

int main(int argc, char** argv) {

	WaveList waveList;
	waveList.load(string(argv[1]));

	Camera camera;

	ofstream f("report.txt");

	if (f.is_open()){

		f << "Waves detected: " << waveList.getSize() << endl << endl;

		for (int i = 0; i < waveList.getSize(); i++ ) {

			Wave wave = waveList.getWave(i);

			f << "Wave index " << i << endl;
			f << "From point " << wave.bottom.getY() << " to " << wave.top.getY() << endl;
			f << "Height (pixels): " << wave.bottom.getY() - wave.top.getY() << endl;
			f << "Height (meters): " << camera.calculateRealHeight(wave.bottom.getY(),wave.top.getY()) << endl << endl;

		}

	}

	f.close();

	
	return 0;
}