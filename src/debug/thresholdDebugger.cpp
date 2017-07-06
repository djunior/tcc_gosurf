#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "imageFilter.h"
#include "filterPipeline.h"
#include "imageOutput.hpp"
#include "gaussianBlurFilter.h"
#include "skyRemoverFilter.hpp"
#include "waveBandDebugger.hpp"
#include "cannyFilter.h"
#include "thresholdFilter.hpp"
#include "waveBandFinder.h"
#include "histogramEqualizatorFilter.h"
#include "sobelFilter.h"

using namespace std;
using namespace cv;
using namespace tcc;

int blur_slider = 15;
int blur_slider_max = 29;

int threshold_slider = 150;
int threshold_slider_max = 255;

Mat greyImage;
Mat outputMat;

// FilterPipeline skyDetector;

void processSobel(Mat& mat) {
	FilterPipeline auxPipeline(mat);
	auxPipeline.addFilter(new GaussianBlurFilter(3));
	// auxPipeline.addFilter(new ThresholdFilter(threshold_slider,1,255));
	auxPipeline.addFilter(new SobelFilter());
	auxPipeline.addFilter(new ImageOutput("threshold - thresholded image"));
	auxPipeline.filter();
}

void processThreshold(Mat& mat) {

	cout << "Processing - threshold = " << threshold_slider << ", blur = " << blur_slider << endl;

	FilterPipeline auxPipeline(&mat);

	if (blur_slider % 2 == 0)
		blur_slider++;

	auxPipeline.addFilter(new GaussianBlurFilter(blur_slider));
	auxPipeline.addFilter(new ThresholdFilter(threshold_slider,0,255));
	// auxPipeline.addFilter(new ImageOutput("threshold - thresholded image"));
	auxPipeline.addFilter(new WaveBandFinder(WaveBandFinder::WBF_MODE_THRESHOLD));
	auxPipeline.addFilter(new CannyFilter(0));
	auxPipeline.addFilter(new WaveBandDebugger(mat));
	// auxPipeline.addFilter(new WaveDetector());
	auxPipeline.filter();

	// auxPipeline.getOutputMat()->copyTo(outputMat(Range(auxPipeline.getOutputMat()->rows+10, auxPipeline.getOutputMat()->rows*2+10),Range::all() ));

	auxPipeline.getOutputMat()->copyTo(outputMat);
}

void processWatershed(Mat& mat) {

	cout << "Main Process with threshold value " << threshold_slider << endl;

	FilterPipeline auxPipeline(&mat);
	auxPipeline.addFilter(new GaussianBlurFilter(blur_slider));
	ThresholdFilter* tf = new ThresholdFilter(threshold_slider,4,0);
	auxPipeline.addFilter(tf);
	auxPipeline.addFilter(new ImageOutput("watershed - thresholded image"));
	auxPipeline.addFilter(new CannyFilter(50));
	auxPipeline.filter();

	FilterPipeline waveFinder;
	waveFinder.setSourceMat(auxPipeline.getOutputMat());
	WaveBandFinder* wbf = new WaveBandFinder(WaveBandFinder::WBF_MODE_WATERSHED);
	wbf->setThresholdedImage(tf->getFilteredImage());
	waveFinder.addFilter(wbf);
	waveFinder.addFilter(new CannyFilter(0));
	waveFinder.addFilter(new WaveBandDebugger(mat));
	// auxPipeline.addFilter(new WaveDetector());
	waveFinder.filter();
	

    waveFinder.getOutputMat()->copyTo(outputMat(Range(0, waveFinder.getOutputMat()->rows), Range::all()));

	// imshow("wave_band_debugger",(*));
}

void on_threshold_trackbar( int, void* )
{
	// alpha = (int) alpha_slider/threshold_slider_max ;

	// addWeighted( src1, alpha, src2, beta, 0.0, dst);
	// processWatershed(greyImage);
	processThreshold(greyImage);
	// processSobel(skyDetector.getOutputMat(),greyImage);

	// imshow( "wave_band_debugger", dst );
	Mat mat;
	resize(outputMat,mat,Size(outputMat.cols/2, outputMat.rows/2));

	imshow("wave_band_debugger",mat);
}

void on_blur_trackbar(int, void* ) {
	// processWatershed(greyImage);
	processThreshold(greyImage);

	Mat mat;
	resize(outputMat,mat,Size(outputMat.cols/2, outputMat.rows/2));

	imshow("wave_band_debugger",mat);
}

int main(int argc, char** argv) {

	string path = argv[1];

	Mat image = imread(path);

	// outputMat = Mat::zeros(10+image.rows*2,image.cols,CV_8UC3);
	outputMat = Mat::zeros(image.rows,image.cols,CV_8UC3);

	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	// HistogramEqualizatorFilter hef;
	// hef.setSourceMat(&greyImage);
	// hef.filter();
	// greyImage = hef.getFilteredImage()->clone();

	// skyDetector.setSourceMat(&greyImage);
	// skyDetector.addFilter(new CannyFilter(50));
	// skyDetector.addFilter(new SkyRemoverFilter(greyImage));
	// skyDetector.filter();

	namedWindow("wave_band_debugger");

	createTrackbar( "Threshold_Trackbar", "wave_band_debugger", &threshold_slider, threshold_slider_max, on_threshold_trackbar );

	createTrackbar( "Blur_Trackbar", "wave_band_debugger", &blur_slider, blur_slider_max, on_blur_trackbar );

	on_threshold_trackbar( threshold_slider, 0 );

	waitKey(0);

	return 0;
}