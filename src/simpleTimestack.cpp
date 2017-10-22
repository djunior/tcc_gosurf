#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include "timestack.h"
#include "filterPipeline.h"
#include "tiltFilter.h"
#include "cannyFilter.h"
#include "skyRemoverFilter.hpp"
#include "imageOutput.hpp"

#include "preProcessor.hpp"

using namespace tcc;
using namespace std;
using namespace cv;

void simpleTimestack(VideoCapture& cap) {
	// Mat frame;
	// Timestack t(cap);

	// int count = 0;
	// while(1) {
	// 	count++;
	// 	cap >> frame;
	// 	if (frame.data == NULL)
	// 		break;

	// 	// imshow("frame",frame);
	// 	Mat greyFrame, eqFrame;
	// 	cvtColor(frame,greyFrame,COLOR_BGR2GRAY);

	// 	equalizeHist(greyFrame,eqFrame);
	// 	if (count == 100) {
	// 		imwrite("output_images/simpletimestack/simpletimestack_original.jpg",greyFrame);
	// 		imwrite("output_images/simpletimestack/simpletimestack_eqframe.jpg",eqFrame);
	// 	}

	// 	if (! isInit) {

	// 		CannyFilter cf(50);
	// 		cf.setSourceMat(&greyFrame);
	// 		cf.filter();

	// 		imwrite("output_images/simpletimestack/simpletimestack_canny.jpg",(*cf.getFilteredImage()));

	// 		skyRemover.init((*cf.getFilteredImage()));

	// 		isInit = true;
	// 	}

	// 	skyRemover.process(eqFrame);

	// 	if (count == 100)
	// 		imwrite("output_images/simpletimestack/simpletimestack_sky_removed.jpg",(*skyRemover.getFilteredImage()));

	// 	// t.process((*skyRemover.getFilteredImage()));

	// 	t.process(eqFrame);

	// 	// waitKey(1);
	// }
	// cout << "Saving timestack..." << endl;
	// t.save("output_images/timestack.jpg");
}

void no_process_timestack(std::string path) {
	VideoCapture cap(path);
	Timestack t(cap);
	Mat frame;
	int count = 0;
	while(true) {
		cap >> frame;
		if (frame.data == NULL)
			break;

		count++;

		t.process(frame);
	}
	t.save("output_images/timestack_original.jpg");
}

int main(int argc, char** argv) {

	string path = argv[1];

	// no_process_timestack(path);
	// if (true)
	// 	return 0;

	VideoCapture cap(path);

	clock_t begin = clock();

	PreProcessor pp(cap);
	pp.generateTimestack(cap);

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  	cout << "Pre processor duration: " << elapsed_secs << endl;

	// simpleTimestack(cap);

	return 0;
}