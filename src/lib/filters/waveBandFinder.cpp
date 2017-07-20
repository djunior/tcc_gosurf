#include "waveBandFinder.h"
#include "opencv/cv.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cannyFilter.h"

/*
 * 
 * Based on https://github.com/opencv/opencv/blob/2.4/samples/cpp/watershed.cpp
 * 
 */

using namespace std;
using namespace cv;

namespace tcc {

void WaveBandFinder::setMode(WaveBandFinder::WaveBandFinder_Mode m) {
	mode = m;
}

void WaveBandFinder::setThresholdedImage(Mat* m) {
	thresholdedImage = m->clone();
}

// TODO: Melhorar maquina de estados
void WaveBandFinder::findMarkers(Mat& thresholdedImage, Mat& markerMask) {
	Mat row = thresholdedImage.col( thresholdedImage.cols / 2 );

	int state = 0;
	int state_count = 0;
	int state_threshold = 15;

	for (int i = 0; i < row.rows; i++) {
		switch(state) {
			case 0: {
				float v = (float) row.at<uchar>(i,0)/ 255;
				if (v > 0) {
					state_count++;
					if (state_count > state_threshold) {
						state++;
						state_count = 0;

						Point pt1(thresholdedImage.cols / 2, i), pt2( (thresholdedImage.cols / 2) + 5, i);

						line( markerMask, pt1, pt2, Scalar::all(255), 5, 8, 0 );

						break;
					}
				} else {
					state_count = 0;
				}
				break;
			}
			case 1: {
				float v = (float) row.at<uchar>(i,0)/ 255;
				if (v == 0) {
					state_count++;
					if (state_count > state_threshold) {
						state++;
						state_count = 0;

						Point pt1(thresholdedImage.cols / 2, i), pt2( (thresholdedImage.cols / 2) + 5, i);

						line( markerMask, pt1, pt2, Scalar::all(255), 5, 8, 0 );

						break;
					}
				} else {
					state_count = 0;
				}
				break;
			}
			case 2: {
				float v = (float) row.at<uchar>(i,0)/ 255;
				if (v > 0) {
					state_count++;
					if (state_count > state_threshold) {
						state++;
						state_count = 0;

						Point pt1(thresholdedImage.cols / 2, i), pt2( (thresholdedImage.cols / 2) + 5, i);

						line( markerMask, pt1, pt2, Scalar::all(255), 5, 8, 0 );

						break;
					}
				} else {
					state_count = 0;
				}
				break;
			}
			case 3: {
				float v = (float) row.at<uchar>(i,0)/ 255;
				if (v == 0) {
					state_count++;
					if (state_count > state_threshold) {
						state++;
						state_count = 0;

						Point pt1(thresholdedImage.cols / 2, i), pt2( (thresholdedImage.cols / 2) + 5, i);

						line( markerMask, pt1, pt2, Scalar::all(255), 5, 8, 0 );

						break;
					}
				} else {
					state_count = 0;
				}
				break;
			}
		}
	}
}

void WaveBandFinder::drawMarkers(cv::Mat& markerMask, cv::Mat& markers) {
	compCount = 0;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(markerMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	if( contours.empty() )
		return;

    int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
        drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
}

void WaveBandFinder::watershed(cv::Mat& filteredImage, cv::Mat& markers) {
	cv::watershed( filteredImage, markers );
}

void WaveBandFinder::createSegments(cv::Mat& markers) {

}

void WaveBandFinder::findByWatershed() {
	Mat markers(thresholdedImage.size(),CV_32S);
	Mat markerMask(thresholdedImage.size(),CV_8UC1);

	markerMask = Scalar::all(0);
	markers = Scalar::all(0);

	findMarkers(thresholdedImage,markerMask);

	Mat debug;

	drawMarkers(markerMask,markers);

	Mat coloredMat;
	cvtColor(srcMat,coloredMat,COLOR_GRAY2BGR);
	watershed(coloredMat,markers);

	Mat m(thresholdedImage.size(),CV_8UC1);
	filteredMat = m.clone();

    for(int i = 0; i < markers.rows; i++ )
	    for(int j = 0; j < markers.cols; j++ )
	    {
	        int index = markers.at<int>(i,j);
			if (index <= 0 || index > compCount)
	        	filteredMat.at<uchar>(i,j) = 0;
	        else
				if (index == 3)
		        	filteredMat.at<uchar>(i,j) = 255;
	    }
}

void WaveBandFinder::findByThreshold() {
	vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    double largest_area;
    int largest_contour_index;

    CannyFilter cf;
    cf.setSourceMat(&srcMat);
    cf.filter();

    imwrite("output_images/process/process_waveband_canny.jpg",(*cf.getFilteredImage()));

    findContours( srcMat, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	for (int i = 0; i< contours.size(); i++) {
       double a = contourArea(contours[i],false);
       if (a > largest_area) {
	       largest_area = a;
	       largest_contour_index = i;
	    }
	}

    Scalar color( 255, 255, 255 );

    Mat dst(srcMat.rows,srcMat.cols,CV_8UC1,Scalar::all(0));

	drawContours( dst, contours, largest_contour_index, color, 1, 8 );

	filteredMat = dst.clone();
}

void WaveBandFinder::filter() {
	switch(mode) {
		case WBF_MODE_WATERSHED:
			findByWatershed();
			break;
		case WBF_MODE_THRESHOLD:
			findByThreshold();
			break;
	}
}

}