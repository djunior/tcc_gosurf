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

bool WaveDetector::detectWave(Trajectory& t, int bottom, int top) {
	int MIN_HEIGHT_THRESHOLD = 10;
	int MAX_HEIGHT_THRESHOLD = 200;
	int height = t.calculateHeight(bottom,top);

	if ((height > MIN_HEIGHT_THRESHOLD && height < MAX_HEIGHT_THRESHOLD)) {
		if (waves.size() == 0 || t.getPoint(bottom).getX() > waves.back().top.getX()) {
			Wave wave(t.points[bottom],t.points[top]);
			waves.push_back(wave);
			return true;
		}
	}

	return false;
}

void WaveDetector::drawWaves(Mat &mat) {
	// cout << "Drawing " << waves.size() << " waves" << endl;

	double average_sum = 0;

	if (waves.size() > 0) {
		for (int i = 0; i < waves.size(); i++) {

			// cout << "Drawing wave " << i << endl;

			tcc::Point bottom = waves[i].bottom;
			tcc::Point top = waves[i].top;

			// cout << "Point 1(" << bottom.x << "," << bottom.y << ")" << endl;
			// cout << "Point 2(" << top.x << "," << top.y << ")" << endl;

			cv::Point pt1(top.getX(), bottom.getY());
			cv::Point pt2(top.getX(), top.getY());
			cv::Point pt3(bottom.getX(), bottom.getY());

			line(mat,pt1,pt2,Scalar(0,0,255),3);
			line(mat,pt3,pt2,Scalar(0,255,0),3);


			average_sum += bottom.getY();
		}
	}

	// double average_y = average_sum / waves.size();

	// cv::Point avP1(0,average_y);
	// cv::Point avP2(mat.cols,average_y);

	// line(mat,avP1,avP2,Scalar(255,255,0),1);
}

void draw(Mat &mat, Trajectory &original, Derivable &derivative, int offset) {
	for (int i = 0; i < derivative.points.size(); i++) {
		tcc::Point p = derivative.points[i];

		if (p.getY() < 0)
			mat.at<Vec3b>(original.getPoint(i+offset).getY(),original.getPoint(i+offset).getX()) = Vec3b(0,0,255);
		else if (p.getY() > 0)
			mat.at<Vec3b>(original.getPoint(i+offset).getY(),original.getPoint(i+offset).getX()) = Vec3b(0,255,0);
		else
			mat.at<Vec3b>(original.getPoint(i+offset).getY(),original.getPoint(i+offset).getX()) = Vec3b(0,0,0);
	}
}

void drawDerivative(Mat &m,Trajectory& original) {
	Mat mat(m.rows,m.cols,CV_8UC3,Scalar::all(255));
	Mat mat2(m.rows,m.cols,CV_8UC3,Scalar::all(255));

	Derivable derivative;
	original.calculateDerivative(derivative);

	draw(mat,original,derivative,1);
	imwrite("output_images/derivative.jpg",mat);

	Derivable secondDerivative;
	derivative.calculateDerivative(secondDerivative);

	draw(mat2,original,secondDerivative,2);
	imwrite("output_images/derivative2.jpg",mat2);

}

void WaveDetector::analyseTrajectory(Trajectory &t) {

	drawDerivative(srcMat,t);

	Derivable derivative;

	t.calculateDerivative(derivative);

	cout << "Derivative size: " << derivative.points.size() << endl;

	int state = 0; 
	int bottom_index = 0;
	int bottom_back_index = 0;
	int top_index = 0;
	int gap_count = 0;
	
	int sea_level_y = 0;
	int sea_level_count = 0;

	// bool adjusted = false;

	int GAP_THRESHOLD = 20;

	for (int i = 1; i < derivative.points.size(); i++) {
		int dX = derivative.getPoint(i).getX();
		int dY = derivative.getPoint(i).getY();

		switch(state) {
			case 0:
				if (dY < 0) {
					state = 1;
					bottom_index = i+1;
					// plateau_count = 0;
				// } else if (dY == 0) {
				// 	plateau_count++;
				// 	if (plateau_count > 30 && ! adjusted) {
				// 		if (waves.size() > 0) {
				// 			adjusted = true;
				// 			int averageY = (waves.back().bottom.getY() + t.points[i+1].getY())/2;
				// 			cout << "Adjusting last wave bottom: " << waves.back().bottom.getY() << " to " << averageY << endl;
				// 			waves.back().bottom = Point(waves.back().bottom.getX(),averageY);
				// 		}
				// 	}
				// } else {
				// 	plateau_count = 0;
				}

				break;
			case 1:
				if (dY > 0) {
					top_index = i+1;
					state = 2;
				}

				break;
			case 2:
				if (dY > 0) {
					
					state = 3;
					gap_count = 0;

				} else {
					if (t.calculateHeight(bottom_index,top_index) < t.calculateHeight(bottom_index,i+1))
						top_index = i+1;
				}

				break;
			case 3:

				if (gap_count >= GAP_THRESHOLD) {
					
					if (detectWave(t,bottom_index,top_index)) {

						if (waves.size() > 1 && sea_level_count > 0) {
							cout << "Adjusting sea level" << endl;
							int averageY = sea_level_y / sea_level_count;
							cout << "Old Y " << waves[waves.size()-2].bottom.getY() << endl;
							cout << "averageY " << averageY << endl;
							int x = waves[waves.size()-2].bottom.getX();
							int y = (waves[waves.size()-2].bottom.getY() + averageY)/2;
							cout << "New Y " << y << endl;
							waves[waves.size()-2].bottom = Point(x,y);
						}

						sea_level_y = 0;
						sea_level_count = 0;

					} else {
						// cout << "Bottom Index Y: " << t.getPoint(bottom_index).getY() << endl;
						// cout << "Top Index Y: " << t.getPoint(top_index).getY() << endl;

						sea_level_y += t.getPoint(bottom_index).getY();
						sea_level_count++;
					}

					gap_count = 0;
					state = 0;
					bottom_index = 0;
					bottom_back_index = 0;
					top_index = 0;

				} else {

					if (dY < 0) {
						state = 2;

						if (t.calculateHeight(bottom_index,top_index) < t.calculateHeight(bottom_index,i+1))
							top_index = i+1;

					} else {

						gap_count++;

					}
				}
				break;
		}

		if ( state > 0 && i == (derivative.points.size() - 1) ) {
			if (detectWave(t,bottom_index,top_index)) {
				// if (waves.size() > 1) {
				// 	int averageY = sea_level_y / sea_level_count;
				// 	int x = waves[waves.size()-2].bottom.getX();
				// 	int y = (waves[waves.size()-2].bottom.getY() + averageY)/2;
				// 	waves[waves.size()-2].bottom = Point(x,y);
				// }
			} else {
				// int averageY = sea_level_y / sea_level_count;
				// int x = waves[waves.size()-1].bottom.getX();
				// int y = (waves[waves.size()-1].bottom.getY() + averageY)/2;
				// waves[waves.size()-1].bottom = Point(x,y);
			}

			state = 0;
			gap_count = 0;
			bottom_index = 0;
			bottom_back_index = 0;
			top_index = 0;
		}

	}
}

void WaveDetector::extractWaveDetails() {

	ofstream f;
  	f.open("results.txt");
	
	Camera camera;

	double time_diff_sum = 0;

	double height_sum = 0;
	int height_count = 0;

	int state = 0;

	for (int i = 0; i < waves.size(); i++) {
	
		if (i < (waves.size()-1)) {
			int pixel_distance = waves[i+1].bottom.getX() - waves[i].top.getX();
			double time_distance = pixel_distance / 30;
		}

		tcc::Point bottom = waves[i].bottom;
		tcc::Point top = waves[i].top;

		int halfway = waves[i].getHalfway();

		int height = waves[i].getHeight();

		double realHeight = camera.calculateRealHeight(bottom.getY(),top.getY());

		cout << "Found wave at " << halfway << " height " << height << " real height: " << realHeight << " m" << endl;
		f << height << " px, " << realHeight << " m" << endl;
	}

	f.close();

}

void WaveDetector::filter() {

	int col = 0;

	while(true) {

		if (col >= srcMat.cols-1) {
			break;
		}

		cout << "Detecting new trajectory in Rect(" << col << ", 0, " << srcMat.cols - col << ", " << srcMat.rows << ")" << endl;

		Trajectory t(srcMat,Rect(col,0,srcMat.cols - col, srcMat.rows));
		
		cout << "Analysing the trajectory" << endl;
		analyseTrajectory(t);

		if (t.points.back().getX() >= (srcMat.cols-1) )
			break;
		else
			col = t.points.back().getX()+1;
	}

	extractWaveDetails();

	filteredMat = srcMat.clone();

}

void WaveDetector::save(char* fname) {
	ofstream f;
  	f.open(fname);

  	for (int i = 0; i < waves.size(); i ++) {
  		tcc::Point bottom = waves[i].bottom;
  		tcc::Point top = waves[i].top;

  		f << bottom.getY() << "," << bottom.getX() << endl;
  		f << top.getY() << "," << top.getX() << endl;
  	}

  	f.close();

}

}

