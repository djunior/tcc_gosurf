#ifndef __PRE_PROCESSOR_H__
#define __PRE_PROCESSOR_H__

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "imageProcessor.h"
#include "timestack.h"
#include "cannyFilter.h"
#include "skyRemoverFilter.hpp"
#include "tiltFilter.h"

using namespace std;
using namespace cv;

namespace tcc {

class PreProcessor : public ImageProcessor {
private:
	Timestack timestack;

	bool isInit;
	
	void init(Mat &m) {
		if (isInit)
			return;

		// imwrite("output_images/simpletimestack/simpletimestack_mat.jpg",m);

		CannyFilter cf(50);
		cf.setSourceMat(&m);
		cf.filter();

		// imshow("output_images/simpletimestack/simpletimestack_canny.jpg",(*cf.getFilteredImage()));

		skyRemover.init((*cf.getFilteredImage()));

		isInit = true;
	}

	int count;

public:

	SkyRemoverFilter skyRemover;

	PreProcessor(VideoCapture &cap) : timestack(cap) {
		isInit = false;
	}

	void generateTimestack(VideoCapture &cap) {
		// cout << "Generate timestack" << endl;
		Mat frame;
		count = 0;
		while(true) {
			cap >> frame;
			if (frame.data == NULL)
				break;

			count++;
			if (count == 100)
				imwrite("output_images/simpletimestack/simpletimestack_original.jpg",frame);

			process(frame);
		}
		save("output_images/timestack.jpg");
	}
	
	void process(Mat &m) {
		// cout << "process begin" << endl;
		Mat greyFrame, eqFrame;
		TiltFilter tiltFilter;

		// Passo 1 - converter para nível de cinza
		if (m.channels() == 1)
			greyFrame = m.clone();
		else
			cvtColor(m,greyFrame,COLOR_BGR2GRAY);

		// cout << "calling init" << endl;
		// Passo 2.1 - detecção da linha de horizonte
		init(greyFrame);

		// cout << "equalizing hist" << endl;
		// Passo 2.2 - Equalização de Histograma
		equalizeHist(greyFrame,eqFrame);

		// imshow("eq",eqFrame);

		// cout << "removing sky" << endl;
		// Passo 3 - Remoção do Céu
		skyRemover.process(eqFrame);

		// imshow("sky removed",(*skyRemover.getFilteredImage()));
		// waitKey(0);

		// cout << "initializing tilt filter" << endl;
		tiltFilter.init(skyRemover.getFilteredImage());

		// cout << "fixing tilt" << endl;
		tiltFilter.process((*skyRemover.getFilteredImage()));

		// cout << "adding to timestack" << endl;
		// Passo 4 - adicionar frame ao timestack
		timestack.process((*tiltFilter.getFilteredImage()));

		if (count == 100) {

			imwrite("output_images/simpletimestack/simpletimestack_sky_removed_untilt.jpg",(*tiltFilter.getFilteredImage()));

			tiltFilter.process(m);
			imwrite("output_images/simpletimestack/simpletimestack_original_untilt.jpg",(*tiltFilter.getFilteredImage()));

			CannyFilter cf(50);
			cf.setSourceMat(tiltFilter.getFilteredImage());
			cf.filter();

			imwrite("output_images/simpletimestack/simpletimestack_canny_untilt.jpg",(*cf.getFilteredImage()));

			tiltFilter.process(eqFrame);
			imwrite("output_images/simpletimestack/simpletimestack_eq_untilt.jpg",(*tiltFilter.getFilteredImage()));

			tiltFilter.process(greyFrame);
			imwrite("output_images/simpletimestack/simpletimestack_grey_untilt.jpg",(*tiltFilter.getFilteredImage()));

		}
	}

	void save(string path) {
		// cout << "PreProcessor::save" << endl;
		timestack.save(path);
	}
};

}

#endif