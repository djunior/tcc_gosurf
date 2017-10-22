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
#include "waveList.hpp"
#include <algorithm>

using namespace std;
using namespace cv;
using namespace tcc;

void drawLines(Mat& mat, Wave& wave) {
	line(mat,cv::Point(0,wave.bottom.getY()),cv::Point(mat.cols,wave.bottom.getY()),Scalar(0,0,255),2);
	line(mat,cv::Point(0,wave.top.getY()),cv::Point(mat.cols,wave.top.getY()),Scalar(0,0,255),2);
}

int getWaveFromFrame(WaveList &waves, int frameNumber) {
	for (int i = 0; i < waves.getSize(); i++) {
		if (frameNumber >= min(waves.getWave(i).bottom.getX(),waves.getWave(i).top.getX()) && frameNumber <= max(waves.getWave(i).bottom.getX(),waves.getWave(i).top.getX())) {
			return i;
		}
	}
	return -1;
}

bool watchWave(Mat &frame, WaveList &waves, int frameCount, string name) {
	if(frame.data == NULL)
		return false;

	int index = getWaveFromFrame(waves,frameCount);
	if (index >= 0) {
		// cout << "Found index: " << index << endl;
		Wave wave = waves.getWave(index);
		drawLines(frame,wave);
	}

	// resize(frame,frame,Size(frame.cols/2,frame.rows/2));

	// imshow(name,frame);
	return true;
}

// void watchWaves(VideoCapture &cap, vector<Wave> &waves) {
// 	cap.set(CV_CAP_PROP_POS_FRAMES,0);

// 	int frame_count = 0;
// 	while(true) {
// 		watchWave(cap,waves,frame_count);

// 		frame_count++;

// 		waitKey(10);
// 	}
// }

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

void detectWaveInFrames(VideoCapture &cap, WaveList &waves, string outputPath) {

	PreProcessor preProcessor(cap);

	Mat frame;

	for (int i = 0; i < waves.getSize(); i++) {
		Wave wave = waves.getWave(i);

		tcc::Point bottom = wave.bottom;
		tcc::Point top = wave.top;

		// cout << "Bottom: " << bottom.getX() << ", " << bottom.getY() << endl;
		// cout << "Top: " << top.getX() << ", " << top.getY() << endl;

		cap.set(CV_CAP_PROP_POS_FRAMES,top.getX());
		cap.grab();
		cap.retrieve(frame);

		// cout << "Frame size: " << frame.cols << ", " << frame.rows << endl;
		// imshow("frame",frame);
		// waitKey(0);

		preProcessor.process(frame);

		TiltFilter tf;
		tf.init(preProcessor.skyRemover.getFilteredImage());
		tf.process(frame);

		Mat tiltedFrame = tf.getFilteredImage()->clone();
		Mat lineFrame = tiltedFrame.clone();

		imshow("lineFrame",lineFrame);
		// waitKey(0);

		// cout << "Calling drawLine at frame(" << lineFrame.cols << ", " << lineFrame.rows << ")" << endl;
		drawLines(lineFrame,wave);

		stringstream outputStream;
		outputStream << (outputPath + "/wave_") << (i+1) << ".jpg";
		// cout << "Saving " << outputStream.str() << endl;
		ImageOutput io(outputStream.str());
		io.setSourceMat(&lineFrame);
		io.filter();

		stringstream originalOutputStream;
		originalOutputStream << (outputPath + "/wave_original_") << (i+1) << ".jpg";
		ImageOutput originalIo(originalOutputStream.str());
		originalIo.setSourceMat(&tiltedFrame);
		originalIo.filter();

	}

}

int main(int argc, char** argv) {

	// vector< Wave > waves;

	// readWaveFile(waves);
	WaveList algWaves;
	WaveList manualWaves;

	VideoCapture cap1(argv[1]);
	VideoCapture cap2(argv[1]);

	algWaves.load(argv[2]);
	manualWaves.load(argv[3]);

	// cout << "Saving alg waves" << endl;
	// detectWaveInFrames(cap1,algWaves,"output_images/wave_viewer/alg/");
	// cout << "Saving manual waves" << endl;
	// detectWaveInFrames(cap2,manualWaves,"output_images/wave_viewer/manual/");

	PreProcessor preProcessor(cap1);

	Mat frame1,frame2;

	int frame_count = 0;

	int tp_count = 0;
	int fp_count = 0;
	int tn_count = 0;
	int fn_count = 0;

	while(true) {

		cap1 >> frame1;
		cap2 >> frame2;

		if (frame1.data == NULL || frame2.data == NULL)
			break;

		preProcessor.process(frame1);
		TiltFilter tf;
		tf.init(preProcessor.skyRemover.getFilteredImage());
		tf.process(frame1);

		if (! watchWave( (*tf.getFilteredImage()),algWaves,frame_count,"alg frame"))
			break;

		// cout << "Creating output mat" << endl;
		Mat output(tf.getFilteredImage()->rows*2+10,tf.getFilteredImage()->cols,CV_8UC3);

		Mat top(output,Rect(0,0,tf.getFilteredImage()->cols,tf.getFilteredImage()->rows));
		Mat bottom(output,Rect(0,tf.getFilteredImage()->rows+10,tf.getFilteredImage()->cols,tf.getFilteredImage()->rows));

		// cout << "frame(" << tf.getFilteredImage()->cols << "," << tf.getFilteredImage()->rows << ")" << endl;
		// cout << "output(" << output.cols << "," << output.rows << ")" << endl;

		// cout << "Copying frame to output" << endl;
		tf.getFilteredImage()->copyTo(top);

		preProcessor.process(frame2);
		tf.process(frame2);

		if (! watchWave((*tf.getFilteredImage()),manualWaves,frame_count,"manual frame"))
			break;

		// cout << "Copying frame to output" << endl;
		tf.getFilteredImage()->copyTo(bottom);

		resize(output,output,Size(output.cols/2,output.rows/2));
		imshow("frames",output);

		frame_count++;
		char c = waitKey(5);
		if (c == 'p') {
			char tf = waitKey(0);
			if (tf == 't' || tf == 'f') {
				char pn = waitKey(0);
				if (tf == 't' && pn == 'p') {
					tp_count++;
				} else if (tf == 'f' && pn == 'p') {
					fp_count++;
				} else if (tf == 't' && pn == 'n') {
					tn_count++;
				} else if (tf == 'f' && pn == 'n') {
					fn_count++;
				}
			}
			cout << "Verdadeiro positivo: " << tp_count << endl;
			cout << "Verdadeiro negativo: " << tn_count << endl;
			cout << "Falso positivo: " << fp_count << endl;
			cout << "Falso negativo: " << fn_count << endl;
		}
	}

	cout << "Numero total de ondas: " << manualWaves.getSize() << endl;

	cout << "Verdadeiro positivo: " << tp_count << endl;
	cout << "Falso positivo: " << fp_count << endl;
	cout << "Falso negativo: " << fn_count << endl;
	cout << "Verdadeiro negativo: " << tn_count << endl;

	ofstream f("data_confusion_matrix.txt");

	if (f.is_open()) {

		f << "Verdadeiro positivo: " << tp_count << endl;
		f << "Verdadeiro negativo: " << tn_count << endl;
		f << "Falso positivo: " << fp_count << endl;
		f << "Falso negativo: " << fn_count << endl;

		f.close();
	}

	// Mat frame;
	// cap >> frame;
	// imwrite("output_images/wave_viewer/first_frame.jpg",frame);

	// detectWaveInFrames(cap,waves);

	// watchWaves(cap,waves);

}