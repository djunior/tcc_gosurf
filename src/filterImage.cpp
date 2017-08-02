#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "laplacianFilter.h"
#include "cannyFilter.h"
#include "gaussianBlurFilter.h"
#include "sobelFilter.h"
#include "filterPipeline.h"
#include "imageViewer.hpp"
#include "imageOutput.hpp"
#include "dualImageViewer.hpp"
#include "greyscaleFilter.hpp"
#include "imageSaver.hpp"
#include "thresholdFilter.hpp"
#include "waveBandFinder.h"
#include "tiltFilter.h"
#include "histogramEqualizatorFilter.h"
#include "waveDetector.h"
#include "skyRemoverFilter.hpp"
#include "waveBandDebugger.hpp"

#include <vector>

using namespace tcc;
using namespace std;
using namespace cv;

const int threshold_slider_max = 255;
int threshold_slider;

FilterPipeline skyDetector;
Mat image;

void CannyFilterAux(CannyFilter& filter, int t, string& path) {
	filter.setLowThreshold(t);	
	filter.filter();
	stringstream ss;
	char str[256];
	sprintf(str,"%s_canny%d.jpg",path.c_str(),t);
	string fullPath = str;
	filter.save(fullPath);
}

int getMeanValue(Mat* m) {
	long value = 0;
	int count = 0;
	for (int i = 10; i < m->rows; i++)
		for (int j = 10; j < m->cols; j++) {
			value += (long) m->at<uchar>(i,j);
			count++;
		}

	int mean = value / count;

	cout << "mean " << mean << endl;

	return mean;
}

void main_process(Mat& image, Mat& outputMat) {

	Mat greyImage(image.size(),CV_8UC1);

	cout << "Analysing image with size: " << image.cols << ", " << image.rows << endl;

	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	FilterPipeline pipeline(&greyImage);

	//Main processing
	pipeline.addFilter(new GaussianBlurFilter(15));
	pipeline.addFilter(new ImageOutput("output_images/process/process_gaussian.jpg"));
	pipeline.addFilter(new ThresholdFilter(150,0,255));
	pipeline.addFilter(new ImageOutput("output_images/process/process_threshold.jpg"));
	
	//Wave Band Detection
	pipeline.addFilter(new WaveBandFinder(WaveBandFinder::WBF_MODE_THRESHOLD));

	// Debug
	FilterPipeline* auxPipeline = new FilterPipeline();
	auxPipeline->addFilter(new WaveBandDebugger(image));
	auxPipeline->addFilter(new ImageOutput("output_images/process/process_waveband.jpg"));
	pipeline.addFilter(auxPipeline);

	pipeline.addFilter(new ImageOutput("output_images/process/process_breakzone.jpg"));

	pipeline.filter();

	pipeline.getOutputMat()->copyTo(outputMat);
}

void detectAnalyzeWaves(Mat& originalMat, Mat& breakzoneMat) {
	cout << "Detect and analyze waves" << endl;
	ImageOutput breakzoneOutput("output_images/process/process_breakzone.jpg");
	breakzoneOutput.setSourceMat(&breakzoneMat);
	breakzoneOutput.filter();	

	WaveDetector wd;
	wd.setSourceMat(&breakzoneMat);
	wd.filter();

	Mat wavesTimestack = originalMat.clone();
	wd.drawWaves(wavesTimestack);
	imshow("waves timestack",wavesTimestack);

	Mat wavesBreakzone = breakzoneMat.clone();
	cvtColor(wavesBreakzone,wavesBreakzone,COLOR_GRAY2BGR);
	wd.drawWaves(wavesBreakzone);

	ImageOutput wavesOutput("output_images/process/process_waves_result.jpg");
	wavesOutput.setSourceMat(&wavesTimestack);
	wavesOutput.filter();

	ImageOutput wavesBreakzoneOutput("output_images/process/process_waves_breakzone_result.jpg");
	wavesBreakzoneOutput.setSourceMat(&wavesBreakzone);
	wavesBreakzoneOutput.filter();

	wd.save("waves.txt");

}

int main(int argc, char* argv[]) {

	string path = argv[1];
	
 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	string outputPath = "output_images/" + file_without_extension + "_canny.jpg";

	// std::string canny_path = "output_images/" + file_without_extension;

	image = imread(path);

	// convertToGreyscale(image,greyImage);
	Mat breakzoneMat = Mat::zeros(image.cols,image.rows,CV_8UC1);

	main_process(image,breakzoneMat);
	detectAnalyzeWaves(image,breakzoneMat);
	// compareBlur(image);

	// processMain(&greyImage,image);
	char* mode = getenv ("DEBUG");
	if (! mode || strcmp(mode,"n") != 0)
		waitKey(0);
	if (1 == 1)
		return 0;

	return 0;
}