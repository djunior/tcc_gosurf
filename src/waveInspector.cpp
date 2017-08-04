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

using namespace std;
using namespace cv;
using namespace tcc;

class WaveCreator {
private:
	WaveList waves;
	tcc::Point bottom,top;
	int state;
	int frame;
	bool handleEvents;

	Mat baseMat;

	bool isInsideRadious(tcc::Point p, tcc::Point center, int radious) {
		return (pow(p.getX() - center.getX(),2) + pow(p.getY() - center.getY(),2)) <= pow(radious,2);
	}

	void handleState0(tcc::Point p) {
		top = p;
		state = 1;
	}

	void handleState1(tcc::Point p) {
		bottom = p;
		state = 0;
	}

public:
	WaveCreator() {
		state = 0;
		handleEvents = false;
	}

	void enableEvents(Mat& mat, int f) {
		handleEvents = true;
		baseMat = mat.clone();
		frame = f;
	}

	void disableEvents() {
		handleEvents = false;
		state = 0;
	}

	void clicked(int x, int y) {
		if (! handleEvents)
			return;

		tcc::Point p(x,y);
		switch(state) {
			case 0:
				handleState0(p);
				break;
			case 1:
				handleState1(p);
				break;
		}

		Mat m = baseMat.clone();
		draw(m);
		imshow("frame",m);
	}

	void draw(Mat& mat) {
		line(mat,cv::Point(0,bottom.getY()),cv::Point(mat.cols,bottom.getY()),Scalar(0,255,0),2);
		line(mat,cv::Point(0,top.getY()),cv::Point(mat.cols,top.getY()),Scalar(0,0,255),2);
	}

	void addWave() {
		tcc::Point b(frame,bottom.getY());
		tcc::Point t(frame,top.getY());
		waves.addWave(b,t);
	}
	
	void save(string fname, string info) {
		waves.save(fname,info);
	}

};

void watchWaves(VideoCapture &cap,WaveCreator& waves) {
	Mat frame, tiltedFrame;
	int waveCount = 0;

	cout << "Getting number of frames" << endl;
	int numberOfFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	cout << "Creating pre processor" << endl;
	PreProcessor preProcessor(cap);
	TiltFilter tf;

	while (true) {

		cap >> frame;

		if (frame.data == NULL)
			break;

		preProcessor.process(frame);

		tf.init(preProcessor.skyRemover.getFilteredImage());
		tf.process(frame);

		Mat tiltedFrame = tf.getFilteredImage()->clone();

		char str[200];

		sprintf(str,"%f %%",(float) cap.get(CV_CAP_PROP_POS_FRAMES)*100 / numberOfFrames);
		putText(tiltedFrame, str, Point2f(50,50), CV_FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));

		cout << "Drawing frame" << endl;

		line(tiltedFrame,cv::Point(tiltedFrame.cols/2,0),cv::Point(tiltedFrame.cols/2,tiltedFrame.rows),Scalar(255,255,255));

		imshow("frame",tiltedFrame);

		char c = waitKey(10);
		if (c == 'p') {

			waves.enableEvents(tiltedFrame,cap.get(CV_CAP_PROP_POS_FRAMES));

			stringstream outputStream;

			bool stayOnLoop = true;

			while(stayOnLoop) {
				char key = waitKey(0);
				switch(key) {
					case 's':
						waves.addWave();
						outputStream << "output_images/wave_inspector/wave_" << (++waveCount) << ".jpg";
						imwrite(outputStream.str(),tiltedFrame);
					case 'c':
						stayOnLoop = false;
						break;
					case 'b':
						cap.set(CV_CAP_PROP_POS_FRAMES,cap.get(CV_CAP_PROP_POS_FRAMES)-cap.get(CV_CAP_PROP_FPS));
						cap >> frame;

						preProcessor.process(frame);
						tf.init(preProcessor.skyRemover.getFilteredImage());
						tf.process(frame);
						tiltedFrame = tf.getFilteredImage()->clone();

						if (tiltedFrame.data != NULL) {
							sprintf(str,"%f %%",(float) cap.get(CV_CAP_PROP_POS_FRAMES)*100 / numberOfFrames);
							putText(tiltedFrame, str, Point2f(50,50), CV_FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
							imshow("frame",tiltedFrame);
							waves.enableEvents(tiltedFrame,cap.get(CV_CAP_PROP_POS_FRAMES));
						}
						break;
					case 'f':
						cap.set(CV_CAP_PROP_POS_FRAMES,cap.get(CV_CAP_PROP_POS_FRAMES)+cap.get(CV_CAP_PROP_FPS));
						cap >> frame;

						preProcessor.process(frame);
						tf.init(preProcessor.skyRemover.getFilteredImage());
						tf.process(frame);
						tiltedFrame = tf.getFilteredImage()->clone();

						if (tiltedFrame.data != NULL) {
							sprintf(str,"%f %%",(float) cap.get(CV_CAP_PROP_POS_FRAMES)*100 / numberOfFrames);
							putText(tiltedFrame, str, Point2f(50,50), CV_FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
							imshow("frame",tiltedFrame);
							waves.enableEvents(tiltedFrame,cap.get(CV_CAP_PROP_POS_FRAMES));
						}
						break;
				}

			}

			waves.disableEvents();

		}

	}

}


void mouseHandler(int event, int x, int y, int flags, void* userdata) {
	WaveCreator* waves = (WaveCreator*) userdata;

    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        waves->clicked(x,y);
    }
    // else if  ( event == EVENT_RBUTTONDOWN )
    // {
    //     cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    // }
    // else if  ( event == EVENT_MBUTTONDOWN )
    // {
    //     cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    // }
    // else if ( event == EVENT_MOUSEMOVE )
    // {
    //     cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
    // }
}

int main(int argc, char** argv) {

	VideoCapture cap(argv[1]);

	WaveCreator waveCreator;

	cout << "Creating named window" << endl;
	namedWindow("frame",1);
	cout << "Setting mouse callback" << endl;
	setMouseCallback("frame", mouseHandler, &waveCreator);
	cout << "Watching waves" << endl;
	watchWaves(cap,waveCreator);

	waveCreator.save("manual_waves.txt",string(argv[1]));

	return 0;
}