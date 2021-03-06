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
	if (argc > 2)
		camera.load(string(argv[2]));

	ofstream f("report.txt");

	if (f.is_open()){

		f << "Waves detected: " << waveList.getSize() << endl << endl;

		for (int i = 0; i < waveList.getSize(); i++ ) {

			Wave wave = waveList.getWave(i);

			f << "Wave index " << i << endl;
			f << "From height point " << wave.bottom.getY() << " to " << wave.top.getY() << endl;
			f << "From Duration point " << wave.bottom.getX() << " to " << wave.top.getX() << endl;
			f << "Height (pixels): " << wave.bottom.getY() - wave.top.getY() << endl;
			f << "Height (meters): " << camera.calculateRealHeight(wave.bottom.getY(),wave.top.getY()) << endl << endl;

		}

	}

	f.close();

	
	return 0;
}