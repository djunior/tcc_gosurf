#include "waveDetector.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "camera.hpp"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <unistd.h>
#include <fstream>

using namespace std;
using namespace cv;

namespace tcc {

WaveDetector::~WaveDetector() {
	for (int i = 0; i < trajectoryList.size(); i++) {
		delete trajectoryList[i];
	}
	trajectoryList.clear();
}

void WaveDetector::detectWave(Trajectory& t, int bottom, int top) {
	int MIN_HEIGHT_THRESHOLD = 10;
	int MAX_HEIGHT_THRESHOLD = 200;
	int height = t.calculateHeight(bottom,top);

	// cout << "Detecting wave from " << bottom << " to " << top << endl;
	// cout << "Calculating height: " << height << endl;
	
	if ((height > MIN_HEIGHT_THRESHOLD && height < MAX_HEIGHT_THRESHOLD)) {
		if (waves.size() == 0 || t.getPoint(bottom).y > waves.back().top.y) {
			Wave wave(t.points[bottom],t.points[top]);
			// waves.push_back(t.points[bottom]);
			// waves.push_back(t.points[top]);
			waves.push_back(wave);
		}
	}
}

void WaveDetector::drawWaves(Mat &mat) {
	Camera camera;
	cout << "Drawing " << waves.size() << " waves" << endl;

	double average_sum = 0;

	if (waves.size() > 0) {
		for (int i = 0; i < waves.size(); i++) {

			cout << "Drawing wave " << i << endl;

			Trajectory::Point bottom = waves[i].bottom;
			Trajectory::Point top = waves[i].top;

			cout << "Point 1(" << bottom.x << "," << bottom.y << ")" << endl;
			cout << "Point 2(" << top.x << "," << top.y << ")" << endl;

			Point pt1(top.y, bottom.x);
			Point pt2(top.y, top.x);
			Point pt3(bottom.y, bottom.x);

			line(mat,pt1,pt2,Scalar(0,0,255),3);
			line(mat,pt3,pt2,Scalar(0,255,0),3);

			int halfway = (top.y + bottom.y)/2;
			int height = (bottom.x - top.x);

			double realHeight = camera.calculateRealHeight(bottom.x,top.x);

			average_sum += bottom.x;

			cout << "Found wave at " << halfway << " height " << height << " real height: " << realHeight << " m" << endl;
		}
	}

	double average_y = average_sum / waves.size();

	Point avP1(0,average_y);
	Point avP2(mat.cols,average_y);

	line(mat,avP1,avP2,Scalar(255,255,0),1);

}

void WaveDetector::filter() {
	cout << "Filtering" << endl;

	Trajectory t(srcMat);

	// findFirstPoint(t);
	// fillTrajectory(t);

	Mat debugMat(srcMat.size(),CV_8UC3,Scalar::all(0));

	vector<Trajectory::Point> derivative;

	t.calculateDerivative(derivative);

	// cout << "Derivative size: " << derivative.size() << endl;

	cout << "Detecting waves" << endl;

	int state = 0; 
	int bottom_index = 0;
	int top_index = 0;
	int gap_count = 0;

	int GAP_THRESHOLD = 20;

	for (int i = 1; i < derivative.size(); i++) {
		int dX = derivative[i].x;
		int dY = derivative[i].y;

		debugMat.at<Vec3b>(t.points[i+1].x,t.points[i+1].y) = Vec3b(255,255,255);

		// cout << "Index: " << i << endl;
		// cout << "State: " << state << endl;
		// cout << "dX: " << dX << endl;
		// cout << "bottom index: " << bottom_index << endl;
		// cout << "top index: " << top_index << endl;
		// cout << "gap count: " << gap_count << endl << endl;

		switch(state) {
			case 0:
				if (dX < 0) {
					state = 1;
					bottom_index = i+1;
				}

				break;
			case 1:
				if (dX > 0) {
					top_index = i+1;
					state = 2;
				}

				break;
			case 2:
				if (dX > 0) {
					
					state = 3;
					gap_count = 0;

				} else {
					if (t.calculateHeight(bottom_index,top_index) < t.calculateHeight(bottom_index,i+1))
						top_index = i+1;
				}

				break;
			case 3:

				if (gap_count >= GAP_THRESHOLD) {
					
					detectWave(t,bottom_index,top_index);

					gap_count = 0;
					state = 0;
					bottom_index = 0;
					top_index = 0;

				} else {

					if (dX < 0) {
						state = 2;

						if (t.calculateHeight(bottom_index,top_index) < t.calculateHeight(bottom_index,i+1))
							top_index = i+1;

					} else {

						gap_count++;

					}
				}
				break;
		}

		if ( state > 0 && i == (derivative.size() - 1) ) {
			detectWave(t,bottom_index,top_index);

			state = 0;
			gap_count = 0;
			bottom_index = 0;
			top_index = 0;
		}

	}

	// for (int i = 0; i < waves.size()-1; i = i + 2) {
	// 	if ( (waves[i+1].y - waves[i].y) > 20) {
	// 		cout << "Wave " << (i / 2) + 1 << " low = (" << waves[i].x << "," << waves[i].y << "), high = (" << waves[i+1].x << "," << waves[i+1].y << ")" << endl;

	// 		Point pt1(waves[i].y,waves[i].x);
	// 		Point pt2(waves[i+1].y,waves[i+1].x);

	// 		line(debug2Mat,pt1,pt2,Scalar::all(255));
	// 	}
	// }

	// cout << "Ploting bottom derivative point at (" << dBottom.x << "," << dBottom.y << ")" << endl;
	// cout << "Ploting top derivative point at (" << dTop.x << "," << dTop.y << ")" << endl;

	// line(debugMat, pt1, pt2, Scalar::all(255),5, 8, 0);
	// for (int i = 0; i < srcMat.cols; i++) {
	// 	debugMat.at<Vec3b>(bottom.x,i) = Vec3b(0,0,255);
	// }

	// cout << "Ploting bottom point at (" << bottom.x << "," << bottom.y << ")" << endl;
	// debugMat.at<Vec3b>(bottom.x,bottom.y) = Vec3b(0,0,255);

	// for (int i = 0; i < srcMat.cols; i++) {
	// 	debugMat.at<Vec3b>(top.x,i) = Vec3b(0,255,0);
	// }

	// cout << "Ploting top point at (" << top.x << "," << top.y << ")" << endl;	
	// debugMat.at<Vec3b>(top.x,top.y) = Vec3b(0,255,0);

	// string path = "output_images/filtered/IMG_0732_derivative_debug.jpg";
	// imwrite(path,debugMat);

	filteredMat = srcMat.clone();

	// resize
	resize(debugMat, debugMat, Size(debugMat.cols/2, debugMat.rows/2));
	// imshow("debug mat",debugMat);

	// resize(debug2Mat, debug2Mat, Size(debug2Mat.cols/2, debug2Mat.rows/2));
	// imshow("debug mat 2",debug2Mat);
	// 


	// waitKey(0);

}

void WaveDetector::save(char* fname) {
	ofstream f;
  	f.open(fname);

  	for (int i = 0; i < waves.size(); i ++) {
  		Trajectory::Point bottom = waves[i].bottom;
  		Trajectory::Point top = waves[i].top;

  		f << bottom.x << "," << bottom.y << endl;
  		f << top.x << "," << top.y << endl;
  	}

  	f.close();

}

}

