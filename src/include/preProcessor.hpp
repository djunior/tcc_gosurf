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

		CannyFilter cf(50);
		cf.setSourceMat(&m);
		cf.filter();

		imwrite("output_images/simpletimestack/simpletimestack_canny.jpg",(*cf.getFilteredImage()));

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
		cout << "Generate timestack" << endl;
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
		Mat greyFrame, eqFrame;
		TiltFilter tiltFilter;

		// Passo 1 - converter para nível de cinza
		if (m.channels() == 1)
			greyFrame = m.clone();
		else
			cvtColor(m,greyFrame,COLOR_BGR2GRAY);

		// Passo 2.1 - detecção da linha de horizonte
		init(greyFrame);

		// Passo 2.2 - Equalização de Histograma
		equalizeHist(greyFrame,eqFrame);

		// Passo 3 - Remoção do Céu
		skyRemover.process(eqFrame);

		tiltFilter.init(skyRemover.getFilteredImage());
		tiltFilter.process((*skyRemover.getFilteredImage()));

		// Passo 4 - adicionar frame ao timestack
		timestack.process((*tiltFilter.getFilteredImage()));
		// timestack.process(eqFrame);

		if (count == 100)
			imwrite("output_images/simpletimestack/simpletimestack_sky_removed.jpg",(*skyRemover.getFilteredImage()));
	}

	void save(string path) {
		cout << "PreProcessor::save" << endl;
		timestack.save(path);
	}
};

}

#endif