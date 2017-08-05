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
#include "wave.hpp"

using namespace std;
using namespace cv;
using namespace tcc;

void drawLines(Mat& mat, Wave& wave) {
	line(mat,cv::Point(0,wave.bottom.getY()),cv::Point(mat.cols,wave.bottom.getY()),Scalar(0,0,255),2);
	line(mat,cv::Point(0,wave.top.getY()),cv::Point(mat.cols,wave.top.getY()),Scalar(0,0,255),2);
}

int getWaveFromFrame(vector<Wave> &waves, int frameNumber) {
	for (int i = 0; i < waves.size(); i++) {
		if (frameNumber >= waves[i].bottom.getX() && frameNumber <= waves[i].top.getX()) {
			return i;
		}
	}
	return -1;
}

void watchWaves(VideoCapture &cap, vector<Wave> &waves) {
	cap.set(CV_CAP_PROP_POS_FRAMES,0);
	Mat frame;
	int frame_count = 0;
	while(true) {
		cap >> frame;

		if(frame.data == NULL)
			break;

		int index = getWaveFromFrame(waves,frame_count);
		if (index >= 0)
			drawLines(frame,waves[index]);

		imshow("frame",frame);

		frame_count++;

		waitKey(10);
	}
}

void readWaveFile(vector<Wave> &waves) {

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

	      waves.push_back(Wave(bottom,top));
	    }
	    f.close();
	}

}

void detectWaveInFrames(VideoCapture &cap, vector<Wave> &waves) {

	PreProcessor preProcessor(cap);

	Mat frame;

	for (int i = 0; i < waves.size(); i++) {

		tcc::Point bottom = waves[i].bottom;
		tcc::Point top = waves[i].top;

		cap.set(CV_CAP_PROP_POS_FRAMES,top.getX());
		cap.grab();
		cap.retrieve(frame);

		preProcessor.process(frame);
		TiltFilter tf;
		tf.init(preProcessor.skyRemover.getFilteredImage());
		tf.process(frame);

		Mat tiltedFrame = tf.getFilteredImage()->clone();
		Mat lineFrame = tiltedFrame.clone();

		drawLines(lineFrame,waves[i]);

		stringstream outputStream;
		outputStream << "output_images/wave_viewer/wave_" << (i+1) << ".jpg";
		ImageOutput io(outputStream.str());
		io.setSourceMat(&lineFrame);
		io.filter();

		stringstream originalOutputStream;
		originalOutputStream << "output_images/wave_viewer/wave_original_" << (i+1) << ".jpg";
		ImageOutput originalIo(originalOutputStream.str());
		originalIo.setSourceMat(&tiltedFrame);
		originalIo.filter();

	}

}

int main(int argc, char** argv) {

	vector< Wave > waves;

	readWaveFile(waves);

	VideoCapture cap(argv[1]);

	Mat frame;
	cap >> frame;
	imwrite("output_images/wave_viewer/first_frame.jpg",frame);

	detectWaveInFrames(cap,waves);

	// watchWaves(cap,waves);

}