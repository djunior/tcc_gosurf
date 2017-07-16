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

uchar calculateThresholdValue(Mat* originalMat, Mat* maskMat) {
	int thresholdValue = 255;
	for (int i = 0; i < originalMat->cols; i++) {
		for (int j = 300; j < originalMat->rows; j++) {
			if (maskMat->at<uchar>(j,i) > 0) {
				uchar value = originalMat->at<uchar>(j,i);
				// cout << "Comparing " << (int) value << " with " << (int) thresholdValue << endl;
				if (value < thresholdValue && value > 0)
					thresholdValue = value;
				break;
			}
		}
	}
	cout << "Returning thresholdValue: " << (int) thresholdValue << endl;
	return thresholdValue;
}

void convertToGreyscale(Mat& input, Mat& output) {
	for (int i = 0; i < input.cols; i++) {
		for (int j = 0; j < input.rows; j++) {
			Vec3b bgrValue = input.at<Vec3b>(j,i);
			float b = (float) bgrValue[0] / 255;
			float g = (float) bgrValue[1] / 255;
			float r = (float) bgrValue[2] / 255;
			float value = 255 * (0.35*r + 0.5*g + 0.15*b);
			// cout << "Value: " << value << endl;
			output.at<uchar>(j,i) = (uchar) value;
		}
	}
}

void compareBlur(Mat& image) {
	Mat greyImage(image.size(),CV_8UC1);

	cout << "Analysing image with size: " << image.cols << ", " << image.rows << endl;

	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	FilterPipeline pipeline(&greyImage);

	//Main processing
	pipeline.addFilter(new GaussianBlurFilter(15));
	pipeline.addFilter(new ThresholdFilter(210,0,255));
	pipeline.addFilter(new ImageOutput("output_images/process/process_threshold_120.jpg"));
	
	//Wave Detection
	pipeline.addFilter(new WaveBandFinder(WaveBandFinder::WBF_MODE_THRESHOLD));
	
	// Debug
	FilterPipeline* auxPipeline = new FilterPipeline();
	auxPipeline->addFilter(new WaveBandDebugger(image));
	auxPipeline->addFilter(new ImageOutput("output_images/process/process_waveband_120.jpg"));
	pipeline.addFilter(auxPipeline);

	WaveDetector* wd = new WaveDetector();
	pipeline.addFilter(wd);

	pipeline.filter();

	Mat debug = image.clone();
	wd->drawWaves(debug);

	ImageOutput wavesOutput("output_images/process/process_waves_result_120.jpg");
	wavesOutput.setSourceMat(&debug);
	wavesOutput.filter();

	wd->save("waves.txt");	
}

void process(Mat& image) {

	Mat greyImage(image.size(),CV_8UC1);

	cout << "Analysing image with size: " << image.cols << ", " << image.rows << endl;

	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	FilterPipeline pipeline(&greyImage);

	//Main processing
	pipeline.addFilter(new GaussianBlurFilter(15));
	pipeline.addFilter(new ImageOutput("output_images/process/process_gaussian.jpg"));
	pipeline.addFilter(new ThresholdFilter(150,0,255));
	pipeline.addFilter(new ImageOutput("output_images/process/process_threshold.jpg"));
	
	//Wave Detection
	pipeline.addFilter(new WaveBandFinder(WaveBandFinder::WBF_MODE_THRESHOLD));
	
	// Debug
	FilterPipeline* auxPipeline = new FilterPipeline();
	auxPipeline->addFilter(new WaveBandDebugger(image));
	auxPipeline->addFilter(new ImageOutput("output_images/process/process_waveband.jpg"));
	pipeline.addFilter(auxPipeline);

	WaveDetector* wd = new WaveDetector();
	pipeline.addFilter(wd);

	pipeline.filter();

	Mat debug = image.clone();
	wd->drawWaves(debug);

	ImageOutput wavesOutput("output_images/process/process_waves_result.jpg");
	wavesOutput.setSourceMat(&debug);
	wavesOutput.filter();

	wd->save("waves.txt");
}

// void completeProcess(Mat& image) {
// 	Mat greyImage(image.size(),CV_8UC1);
// 	cvtColor(image,greyImage,COLOR_BGR2GRAY);

// 	FilterPipeline filterPipeline;
// 	filterPipeline.setSourceMat(&greyImage);

// 	// Pre Processing

// }

int main(int argc, char* argv[]) {

	string path = argv[1];
	
 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	string outputPath = "output_images/" + file_without_extension + "_canny.jpg";

	// std::string canny_path = "output_images/" + file_without_extension;

	image = imread(path);

	// convertToGreyscale(image,greyImage);

	process(image);
	compareBlur(image);

	// processMain(&greyImage,image);
	char* mode = getenv ("DEBUG");
	if (! mode || strcmp(mode,"n") != 0)
		waitKey(0);
	if (1 == 1)
		return 0;

	// cout << "Creating sky detector" << endl;

	// skyDetector.setSourceMat(&greyImage);
	
	// // GaussianBlurFilter* gFilter = new GaussianBlurFilter(15);
	
	// // skyDetector.addFilter(gFilter);
	// string path_original = "skydetector_original_image.jpg";
	// skyDetector.addFilter(new ImageSaver(path_original));

	// // skyDetector.addFilter(new LaplacianFilter());

	// CannyFilter *cFilter = new CannyFilter(50);
	// skyDetector.addFilter(cFilter);

	// string path_after_canny = "skydetector_after_canny.jpg";
	// skyDetector.addFilter(new ImageSaver(path_after_canny));

	// SkyRemoverFilter* srf = new SkyRemoverFilter(&greyImage);

	// skyDetector.addFilter(srf);

	// string path_sky_removed = "skydetector_removed.jpg";
	// skyDetector.addFilter(new ImageSaver(path_sky_removed));

	// // skyDetector.addFilter(new ImageViewer("after canny"));
	
	// skyDetector.filter();

	// // uchar thresholdValue = calculateThresholdValue(&greyImage, cFilter->getFilteredImage() );
	// // uchar thresholdValue = calculateThresholdValue(gFilter->getFilteredImage(), cFilter->getFilteredImage() );

	// // if (1 == 1) {
	// // 	waitKey(0);
	// // 	return 0;
	// // }

	// GaussianBlurFilter gbf(15);
	// // gbf.setSourceMat(&image);
	// // gbf.filter();());

	// // imshow("blurred",*gbf.getFilteredImage());
	// // int mean = getMeanValue(gbf.getFilteredImage

	// FilterPipeline mainPipeline(srf->getFilteredImage());

	// mainPipeline.addFilter(new ImageViewer("original image"));

	// mainPipeline.addFilter(new GaussianBlurFilter(15));

	// mainPipeline.addFilter(new ImageViewer("after gaussian 1"));

	// ThresholdFilter* tf = new ThresholdFilter(150);
	// mainPipeline.addFilter(tf);

	// if (argc > 2) {
	// 	cout << "Salvando resultado no filesystem" << endl;
	// 	outputPath = argv[2];
	// 	string path = outputPath + file_without_extension + "_threshold.jpg";
	// 	mainPipeline.addFilter(new ImageSaver(path));
	// } else {
	// 	cout << "Exibindo resultado em um image viewer" << endl;
	// 	mainPipeline.addFilter(new ImageViewer("threshold"));
	// }

	// // mainPipeline.filter();

	// // mainPipeline.addFilter(new GaussianBlurFilter(3));

	// // LaplacianFilter *lf = new LaplacianFilter();
	// // mainPipeline.addFilter(lf);

	// // mainPipeline.addFilter(new ImageViewer("after laplacian filter"));

	// // mainPipeline.addFilter(new GaussianBlurFilter(3));

	// // mainPipeline.addFilter(new ImageViewer("blurring and sharpening output"));
	// // mainPipeline.addFilter(new LaplacianFilter());
	// // mainPipeline.addFilter(new ImageViewer("laplacian output"));
	// // mainPipeline.
	
	// // mainPipeline.addFilter(new ImageViewer("canny output"));
	// if (argc > 2) {
	// 	cout << "Salvando resultado no filesystem" << endl;
	// 	string path = outputPath + file_without_extension + "_canny.jpg";
	// 	mainPipeline.addFilter(new ImageSaver(path));
	// } else {
	// 	cout << "Exibindo resultado em um image viewer" << endl;
	// 	mainPipeline.addFilter(new ImageViewer("pipeline output"));
	// }

	// // mainPipeline.addFilter(new LaplacianFilter());
	// // mainPipeline.addFilter(new GaussianBlurFilter(9));
	// // mainPipeline.addFilter(&cf);

	// // FilterPipeline pipelineCanny0;
	// // pipelineCanny0.addFilter(new CannyFilter(0));
	// // pipelineCanny0.addFilter(new ImageViewer("canny 0"));
	// // mainPipeline.addFilter(&pipelineCanny0);

	// // FilterPipeline pipelineCanny10;
	// // pipelineCanny10.addFilter(new CannyFilter(10));
	// // pipelineCanny10.addFilter(new ImageViewer("canny 10"));
	// // mainPipeline.addFilter(&pipelineCanny10);

	// // FilterPipeline pipelineCanny20;
	// // pipelineCanny20.addFilter(new CannyFilter(20));
	// // pipelineCanny20.addFilter(new ImageViewer("canny 20"));
	// // mainPipeline.addFilter(&pipelineCanny20);

	// // FilterPipeline pipelineCanny50;
	// // pipelineCanny50.addFilter(new CannyFilter(50));
	// // pipelineCanny50.addFilter(new ImageViewer("canny 50"));
	// // mainPipeline.addFilter(&pipelineCanny50);

	// // ImageViewer iv("filtered")


	// // CannyFilter cf50;
	// // cf50.setLowThreshold(50);

	// mainPipeline.filter();

	// // processMain(skyDetector.getOutputMat(),image);

	// // namedWindow("bar");
	// // createTrackbar( "Trackbar", "wave_band_debugger", &threshold_slider, threshold_slider_max, on_trackbar );
	// /// Show some stuff
	// // imshow("bar",img);
	// // on_trackbar( threshold_slider, 0 );

	// if (argc > 2) {
	// 	string path = outputPath + file_without_extension + "_debug.jpg";
	// 	imwrite(path,image);
	// } else {
	// 	// resize(image, image, Size(image.cols/2, image.rows/2));
	// 	// imshow("debug",image);
	// 	waitKey(0);
	// }


	// FilterPipeline waveBandPipeline(mainPipeline.getOutputMat());

	// // waveBandPipeline.setSourceMat();

	// waveBandPipeline.addFilter(new CannyFilter(50));

	// waveBandPipeline.addFilter(new ImageViewer("after canny 50"));

	// WaveBandFinder* wbf = new WaveBandFinder();
	// // wbf.setSourceMat(cf->getFilteredImage());
	// wbf->setThresholdedImage(mainPipeline.getOutputMat());

	// waveBandPipeline.addFilter(wbf);

	// waveBandPipeline.addFilter(new ImageViewer("after waveband pipeline"));

	// CannyFilter* wbcf = new CannyFilter(0);
	// waveBandPipeline.addFilter(wbcf);

	// waveBandPipeline.addFilter(new ImageViewer("after canny 0"));	

	// // waveBandPipeline.addFilter(new ImageViewer("after canny 0"));

	// if (argc > 2) {
	// 	cout << "Salvando resultado no filesystem" << endl;
	// 	string path = outputPath + file_without_extension + "_wave_band.jpg";
	// 	waveBandPipeline.addFilter(new ImageSaver(path));
	// } else {
	// 	cout << "Exibindo resultado em um image viewer" << endl;
	// 	waveBandPipeline.addFilter(new ImageViewer("wave band pipeline output"));
	// }

	// WaveDetector* wd = new WaveDetector();
	// waveBandPipeline.addFilter(wd);

	// // waveBandPipeline.addFilter(new ImageViewer("result"));

	// waveBandPipeline.filter();

	// // Mat outputMat,sumMat;

	// // Mat waveBandMat = wbcf->getFilteredImage()->clone();

	// // vector<vector<Point> > contours;
	// // vector<Vec4i> hierarchy;

	// // findContours(waveBandMat, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	// // for (int i = 0; i < contours.length; i++) {
	// // 	cout << "" << endl;
	// // }

	// Mat colorMat;

	// // cout << "waveBand type " << waveBandMat.type() << endl;
	// // cvtColor(image,colorMat,COLOR_GRAY2BGR);

	// // if (wd->waves.size() > 0) {
	// // 	cout << "Painting lines on original image" << endl;
	// // 	for (int i = 0; i < (wd->waves.size()-1); i += 2) {
	// // 		Point pt1(wd->waves[i].y, wd->waves[i].x);
	// // 		Point pt2(wd->waves[i+1].y, wd->waves[i+1].x);
	// // 		line(image,pt1,pt2,Scalar(0,0,255));

	// // 		int halfway = (wd->waves[i+1].y + wd->waves[i].y)/2;

	// // 		cout << "Found wave at " << halfway << " height " << wd->waves[i].x - wd->waves[i+1].x << endl;
	// // 	}
	// // }
	// wd->drawWaves(image);

	// // cout << "Colored mat type " << colorMat.type() << endl;

	// // Mat redMat(waveBandMat.size(),CV_8UC3);
	// // redMat = Scalar(0,0,255);

	// // cout << "Creating output mat" << endl;
	// // outputMat = wd->getFilteredImage()->clone();

	// // cout << "Creating sum mat" << endl;
	// // sumMat = image + outputMat;
	// // // addWeighted(image,0.8,outputMat,1.0,sumMat);

	// if (argc > 2) {
	// 	string path = outputPath + file_without_extension + "_debug.jpg";
	// 	imwrite(path,image);
	// } else {
	// 	resize(image, image, Size(image.cols/2, image.rows/2));
	// 	imshow("debug",image);
	// }


	// if (argc <= 2)
	// 	waitKey(0);

	return 0;
}