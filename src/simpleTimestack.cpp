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

class PreProcessor : ImageProcessor {
private:
	Timestack timestack;
	
	SkyRemoverFilter skyRemover;
	
	bool isInit;
	
	void init(Mat &m) {
		if (isInit)
			return;

		CannyFilter cf(50);
		cf.setSourceMat(&m);
		cf.filter();

		imwrite("output_images/simpletimestack/simpletimestack_canny.jpg",(*cf.getFilteredImage()));

		skyRemover.init((*cf.getFilteredImage()));

		isInit = true;
	}

public:
	PreProcessor(VideoCapture &cap) : timestack(cap) {
		isInit = false;
	}

	void generateTimestack(VideoCapture &cap) {
		cout << "Generate timestack" << endl;
		Mat frame;
		while(true) {
			cap >> frame;
			if (frame.data == NULL)
				break;

			process(frame);
		}
		save("output_images/timestack.jpg");
	}
	
	void process(Mat &m) {
		Mat greyFrame, eqFrame;

		// Passo 1 - converter para nível de cinza
		cvtColor(m,greyFrame,COLOR_BGR2GRAY);

		// Passo 2.1 - detecção da linha de horizonte
		init(greyFrame);

		// Passo 2.2 - Equalização de Histograma
		equalizeHist(greyFrame,eqFrame);

		// Passo 3 - Remoção do Céu
		skyRemover.process(eqFrame);

		// Passo 4 - adicionar frame ao timestack
		timestack.process((*skyRemover.getFilteredImage()));
	}

	void save(string path) {
		cout << "PreProcessor::save" << endl;
		timestack.save(path);
	}
};

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

int main(int argc, char** argv) {

	string path = argv[1];

	VideoCapture cap(path);

	PreProcessor pp(cap);
	pp.generateTimestack(cap);

	// simpleTimestack(cap);

	return 0;
}