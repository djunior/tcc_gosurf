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

/*---detectWaveBegin---*/
bool WaveDetector::detectWave(Trajectory& t, int bottom, int top) {
	int MIN_HEIGHT_THRESHOLD = 10;
	int MAX_HEIGHT_THRESHOLD = 200;
	int height = t.calculateHeight(bottom,top);

	if ((height > MIN_HEIGHT_THRESHOLD && height < MAX_HEIGHT_THRESHOLD)) {
		if (waves.size() == 0 || t.getPoint(bottom).getX() > waves.back().top.getX()) {
			return true;
		}
	}

	return false;
}
/*---detectWaveEnd---*/

void drawWavePoints(Mat &m, WaveList& waveList) {
	for (int i = 0; i < waveList.getSize(); i++) {
		Wave wave = waveList.getWave(i);
		circle(m, cv::Point(wave.bottom.getX(),wave.bottom.getY()), 5, Scalar( 0, 0, 255 ), -1, 8 );
		circle(m, cv::Point(wave.top.getX(),wave.top.getY()), 5, Scalar( 0, 255, 0 ), -1, 8 );
	}
}

void WaveDetector::drawWaves(Mat &mat) {
	if (waves.size() > 0) {
		for (int i = 0; i < waves.size(); i++) {
			tcc::Point bottom = waves[i].bottom;
			tcc::Point top = waves[i].top;

			cv::Point pt1(top.getX(), bottom.getY());
			cv::Point pt2(top.getX(), top.getY());
			cv::Point pt3(bottom.getX(), bottom.getY());

			line(mat,pt1,pt2,Scalar(0,255,0),3);
			// line(mat,pt3,pt2,Scalar(0,0,255),3);
		}
	}

	drawWavePoints(mat,rejectedWaveList);
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

/*---fixBottomPointBegin---*/
void fixBottomPoint(vector<Wave> &waves, int sea_level_y) {
	if (waves.size() > 0) {
		int y = (waves.back().bottom.getY() + sea_level_y*2)/3;
		waves.back().bottom = Point(waves.back().bottom.getX(),y);
	}
}
/*---fixBottomPointEnd---*/


/*---analyseTrajectoryBegin---*/
void WaveDetector::analyseTrajectory(Trajectory &t) {
	// A variavel waves é um vector<Wave> definido como propriedade
	// da classe WaveDetector

	Derivable derivative;
	t.calculateDerivative(derivative);

	drawDerivative(srcMat,t);

	int state = 0; 
	int bottom_index = 0;
	int bottom_back_index = 0;
	int top_index = 0;
	int gap_count = 0;
	
	int sea_level_x = 0;
	int sea_level_y = 0;
	int sea_level_count = 0;

	int GAP_THRESHOLD = 20;

	for (int i = 1; i < derivative.points.size(); i++) {
		int dX = derivative.getPoint(i).getX();
		int dY = derivative.getPoint(i).getY();

		switch(state) {
			case 0:
				if (dY < 0) {
					state = 1;
					bottom_index = i+1;
				}

				break;
			case 1:
				if (dY > 0) {
					top_index = i+1;
					state = 3;
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

					if (sea_level_count < 3) {
						sea_level_y += t.getPoint(bottom_index).getY();
						sea_level_count++;
					}

					Wave wave(t.getPoint(bottom_index),t.getPoint(top_index));
					rejectedWaveList.addWave(wave);
					
					if (detectWave(t,bottom_index,top_index)) {

						sea_level_y /= sea_level_count;
						fixBottomPoint(waves,sea_level_y);

						waves.push_back(wave);

						sea_level_y = 0;
						sea_level_count = 0;

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
				Wave wave(t.getPoint(bottom_index),t.getPoint(top_index));
				waves.push_back(wave);
			}

			state = 0;
			gap_count = 0;
			bottom_index = 0;
			bottom_back_index = 0;
			top_index = 0;
		}

	}
}
/*---analyseTrajectoryEnd---*/

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

		height_sum += realHeight;
		height_count++;

		cout << "Found wave at " << halfway << " height " << height << " real height: " << realHeight << " m" << endl;
		f << height << " px, " << realHeight << " m" << endl;
	}

	double averageRealHeight = height_sum / height_count;

	cout << "Average real height: " << averageRealHeight << endl;

	f.close();

}

void WaveDetector::filter() {

	int col = 0;

	srand (time(NULL));

	while(true) {

		if (col >= srcMat.cols-1) {
			break;
		}

		Trajectory t(srcMat,Rect(col,0,srcMat.cols - col, srcMat.rows));
		
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

  		f << bottom.getX() << "," << bottom.getY() << endl;
  		f << top.getX() << "," << top.getY() << endl;
  	}

  	f.close();

}

}

