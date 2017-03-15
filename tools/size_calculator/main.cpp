#include <iostream>

using namespace std;

/*
	Usage: ./main ref_size ref_dist ref_real_size wave_size wave_dist [photo_height] [sensor_size]
*/
int main(int argc, char* argv[]) {
	
	float sensor_size = 1.22e-6;
	float photo_height = 3264;
	float ref_size, ref_dist, ref_real_size, wave_size, wave_dist;

	float scale;

	if (argc < 6) {
		cout << "Usage: ./main ref_size ref_dist ref_real_size wave_size wave_dist [photo_height] [sensor_size]" << endl;
		return 1;
	}

	ref_size = atof(argv[1]);
	ref_dist = atof(argv[2]);
	ref_real_size = atof(argv[3]);

	wave_size = atof(argv[4]);
	wave_dist = atof(argv[5]);

	if (argc > 6) {
		photo_height = atof(argv[6]);
	}

	if (argc > 7) {
		sensor_size = atof(argv[7]);
	}

	float ref_percent = ref_size / photo_height;
	float ref_sensor_size = ref_percent * sensor_size;
	float ref_approximate_size = ref_sensor_size * ref_dist;

	scale = ref_real_size / ref_approximate_size;

	float wave_percent = wave_size / photo_height;
	float wave_sensor_size = wave_percent * sensor_size;
	float wave_approximate_size = wave_sensor_size * scale;

	float wave_real_size = wave_approximate_size * wave_dist;

	cout << "Scale: " << scale << endl;
	cout << "Wave size: " << wave_real_size << endl;

	return 0;
}