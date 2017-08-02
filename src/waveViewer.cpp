#include <iostream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <fstream>

#include "point.hpp"
#include "trajectory.h"
#include "imageOutput.hpp"
#include "filterPipeline.h"
#include "tiltFilter.h"
#include "preProcessor.hpp"

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

	      int bottom_x,bottom_y;
	      int top_x,top_y;

	      sscanf(line_bottom.c_str(),"%i,%i",&bottom_x,&bottom_y);
	      sscanf(line_top.c_str(),"%i,%i",&top_x,&top_y);

	      tcc::Point bottom(bottom_x,bottom_y);
	      tcc::Point top(top_x,top_y);

	      pair<tcc::Point,tcc::Point> wave(bottom,top);
	      waves.push_back(wave);

	      int base,up;

	      double height = calculeSizeByAngle(imageSize, cameraHeight, cameraAngle, focalAngle, bottom_x, top_x );
	    }
	    f.close();
	}

	// Mat frame;
	int frame_count = 0;
	int wave_count = 0;

	PreProcessor preProcessor(cap);

	// while(true) {
	// 	cap >> frame;

	// 	if(frame.data == NULL)
	// 		break;

		for (int i = 0; i < waves.size(); i++) {
			tcc::Point bottom = waves[i].first;
			tcc::Point top = waves[i].second;

			Mat frame;

			cap.set(CV_CAP_PROP_POS_FRAMES,top.getX());
			cap.grab();
			cap.retrieve(frame);

			// imshow("frame",frame);

			// waitKey(0);

			// if (frame_count == top.getX()) {

				// cout << "Frame count: " << frame_count << endl;
				cout << "bottom wave: (" << bottom.getX() << "," << bottom.getY() << ")" << endl;
				cout << "top wave: (" << top.getX() << "," << top.getY() << ")" << endl;

				preProcessor.process(frame);
				TiltFilter tf;
				tf.init(preProcessor.skyRemover.getFilteredImage());
				tf.process(frame);

				Mat tiltedFrame = tf.getFilteredImage()->clone();
				Mat lineFrame = tiltedFrame.clone();

				wave_count++;

				line(lineFrame,cv::Point(0,bottom.getY()),cv::Point(lineFrame.cols,bottom.getY()),Scalar(0,0,255),2);
				line(lineFrame,cv::Point(0,top.getY()),cv::Point(lineFrame.cols,top.getY()),Scalar(0,0,255),2);

				// if ( frame_count == top.getX() ) {
					stringstream outputStream;
					outputStream << "output_images/wave_viewer/wave_" << wave_count << ".jpg";
					cout << "Saving image " << outputStream << endl;
					ImageOutput io(outputStream.str());
					io.setSourceMat(&lineFrame);
					io.filter();

					stringstream originalOutputStream;
					originalOutputStream << "output_images/wave_viewer/wave_original_" << wave_count << ".jpg";
					cout << "Saving image " << originalOutputStream << endl;
					ImageOutput originalIo(originalOutputStream.str());
					originalIo.setSourceMat(&tiltedFrame);
					originalIo.filter();
				// }

				// break;
			// }
		}

		// imshow("frame",frame);

		// frame_count++;

		// waitKey(30);
	// }
	waitKey(0);

}