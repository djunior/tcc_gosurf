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

class WaveBandDebugger : public ImageFilter {
private:
public:
	WaveBandDebugger(Mat& m) : ImageFilter() {
		filteredMat = m.clone();
	}
	WaveBandDebugger(Mat* m) : ImageFilter() {
		filteredMat = m->clone();
	}
	void filter() {
		for (int i = 0; i < srcMat.cols; i++) {
			for (int j = 0; j < srcMat.rows; j++) {
				if (srcMat.at<uchar>(j,i) > 0)
					filteredMat.at<Vec3b>(j,i) = Vec3b(0,0,255);
			}
		}
		// resize(originalMat,originalMat,Size(originalMat.cols/2,originalMat.rows/2));
	}
};

class NewWaveBandFinder : public ImageFilter {
public:
	void filter() {
		cout << "NewWavevBandFinder filter" << endl;
		vector< vector<Point> > contours; // Vector for storing contour
	    vector<Vec4i> hierarchy;
	    double largest_area;
	    int largest_contour_index;

	    cout << "NewWavevBandFinder filter -> calling findContours" << endl;
	    findContours( srcMat, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

		for( int i = 0; i< contours.size(); i++ ) {
	       double a=contourArea( contours[i],false);  //  Find the area of contour
	       if( a>largest_area ){
		       largest_area=a;
		       largest_contour_index=i;                //Store the index of largest contour
		    }
		}

	    Scalar color( 255, 255, 255 );

	    Mat dst(srcMat.rows,srcMat.cols,CV_8UC1,Scalar::all(0));

	    cout << "NewWavevBandFinder filter -> drawing contours" << endl;
 		drawContours( dst, contours,largest_contour_index, color, CV_FILLED, 8, hierarchy );

 		filteredMat = dst.clone();

 		// cout << "Calling imshow" << endl;
  	// 	imshow("NewWaveBandFinder",filteredMat);
	}
};

void processMain(Mat* mat, Mat& originalImage) {

	cout << "Main Process with threshold value " << threshold_slider << endl;

	FilterPipeline auxPipeline(mat);
	auxPipeline.addFilter(new ImageViewer("original image"));
	auxPipeline.addFilter(new GaussianBlurFilter(15));
	auxPipeline.addFilter(new ThresholdFilter(150,0,255));
	auxPipeline.addFilter(new ImageSaver("threshold_image_fail.jpg"));
	// auxPipeline.addFilter(new NewWaveBandFinder());
	// auxPipeline.addFilter(new CannyFilter(0));
	// auxPipeline.addFilter(new WaveBandDebugger(originalImage));
	// auxPipeline.addFilter(new WaveDetector());

	auxPipeline.filter();
}

void process(Mat& image) {

	Mat greyImage(image.size(),CV_8UC1);

	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	FilterPipeline pipeline(&greyImage);

	//Pre-processing
	pipeline.addFilter(new ImageOutput("output_images/process/process_original.jpg"));
	pipeline.addFilter(new CannyFilter(50));
	pipeline.addFilter(new ImageOutput("output_images/process/process_horizon_line.jpg"));
	pipeline.addFilter(new SkyRemoverFilter(&greyImage));
	pipeline.addFilter(new ImageOutput("output_images/process/process_sky_remover.jpg"));

	//Main processing
	pipeline.addFilter(new GaussianBlurFilter(15));
	pipeline.addFilter(new ImageOutput("output_images/process/process_gaussian.jpg"));
	pipeline.addFilter(new ThresholdFilter(150,0,255));
	pipeline.addFilter(new ImageOutput("output_images/process/process_threshold.jpg"));
	pipeline.addFilter(new NewWaveBandFinder());
	pipeline.addFilter(new ImageOutput("output_images/process/process_threshold.jpg"));

	//Wave Detection
	pipeline.addFilter(new CannyFilter(0));
	pipeline.addFilter(new ImageOutput("output_images/process/process_canny.jpg"));

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

void on_trackbar( int, void* )
{
	// alpha = (int) alpha_slider/threshold_slider_max ;

	// addWeighted( src1, alpha, src2, beta, 0.0, dst);
	processMain(skyDetector.getOutputMat(),image);

	// imshow( "wave_band_debugger", dst );
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