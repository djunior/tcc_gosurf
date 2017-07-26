#include <iostream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <fstream>

#include "point.hpp"
#include "trajectory.h"

using namespace std;
using namespace cv;
using namespace tcc;

double calculateAngle(int imageSize, double cameraAngle, double focalAngle, int pixel) {
	return cameraAngle - focalAngle/2 + pixel * (focalAngle / imageSize);
}

double calculeSizeByAngle(int imageSize, double cameraHeight, double cameraAngle, double focalAngle, int waveBase, int waveTop) {
	double angleBase = calculateAngle(imageSize,cameraAngle,focalAngle,waveBase);
	double angleTop = calculateAngle(imageSize,cameraAngle,focalAngle,waveTop);

	double height = cameraHeight * ( 1 - (tan(angleTop) / tan(angleBase)) );

	cout << "Height: " << height << "m" << endl;

	return height;
}

int main(int argc, char** argv) {

	vector< pair<tcc::Point,tcc::Point> > waves;

	VideoCapture cap(argv[1]);

	double cameraHeight = 6.0;
	double cameraAngle = 90.0;
	int imageSize = 720;
	double focalAngle = 60.0;

	string line_bottom;
	string line_top;
	ifstream f("waves.txt");
	if (f.is_open())
	{
		while ( ! f.eof() )
	    {
	      f >> line_bottom;
	      f >> line_top;

	      // cout << "line bottom: " << line_bottom << endl;
	      // cout << "line top: " << line_top << endl;

	      int bottom_x,bottom_y;
	      int top_x,top_y;

	      sscanf(line_bottom.c_str(),"%i,%i",&bottom_x,&bottom_y);
	      sscanf(line_top.c_str(),"%i,%i",&top_x,&top_y);

	      tcc::Point bottom(bottom_x,bottom_y);
	      tcc::Point top(top_x,top_y);

	      pair<tcc::Point,tcc::Point> wave(bottom,top);
	      waves.push_back(wave);

	      int base,up;

	      cout << "Wave point 1: " << bottom_x << endl;
	      cout << "Wave point 2: " << top_x << endl;

	      double height = calculeSizeByAngle(imageSize, cameraHeight, cameraAngle, focalAngle, bottom_x, top_x );
	    }
	    f.close();
	}

	Mat frame;
	int frame_count = 0;
	while(true) {
		cap >> frame;

		if(frame.data == NULL)
			break;

		// Mat frame;
    	// transpose(t_frame,frame);

		for (int i = 0; i < waves.size(); i++) {
			tcc::Point bottom = waves[i].first;
			tcc::Point top = waves[i].second;

			// float a = (top.x - bottom.x)/(top.y - bottom.y);
			// float b = top.x - a*top.y;

			if (frame_count >= bottom.getY() && frame_count <= top.getY()) {
				// int top_y = a*frame_count + b;

				cv::Point bottom_p1(0,bottom.getX());
				cv::Point bottom_p2(frame.cols,bottom.getX());

				line(frame,bottom_p1,bottom_p2,Scalar(0,0,255),2);

				cv::Point top_p1(0,top.getX());
				cv::Point top_p2(frame.cols,top.getX());

				line(frame,top_p1,top_p2,Scalar(0,0,255),2);
			}
		}

		imshow("frame",frame);

		frame_count++;

		waitKey(30);
	}	
}