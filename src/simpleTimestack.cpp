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

using namespace tcc;
using namespace std;
using namespace cv;

void simpleTimestack(VideoCapture& cap) {
	Mat frame;
	Timestack t(cap);

	bool isInit = false;

	SkyRemoverFilter skyRemover;

	while(1) {
		cap >> frame;
		if (frame.data == NULL)
			break;

		// imshow("frame",frame);
		Mat greyFrame, eqFrame;
		cvtColor(frame,greyFrame,COLOR_BGR2GRAY);

		equalizeHist(greyFrame,eqFrame);


		if (! isInit) {
			FilterPipeline pipeline(greyFrame);

			pipeline.addFilter(new CannyFilter(50));

			pipeline.filter();

			CannyFilter cf(50);
			cf.setSourceMat(&greyFrame);
			cf.filter();

			skyRemover.init((*cf.getFilteredImage()));

			isInit = true;
		}

		skyRemover.process(eqFrame);

		t.process((*skyRemover.getFilteredImage()));

		// waitKey(1);
	}
	cout << "Saving timestack..." << endl;
	t.save("output_images/timestack.jpg");
}

int main(int argc, char** argv) {

	string path = argv[1];

	VideoCapture cap(path);

	simpleTimestack(cap);

	return 0;
}