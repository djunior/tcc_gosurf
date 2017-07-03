#ifndef __WAVE_BAND_FINDER_H__
#define __WAVE_BAND_FINDER_H__

#include "imageFilter.h"
#include "opencv/cv.h"

namespace tcc {

using namespace std;

class WaveBandFinder : public ImageFilter {
public:
	// enum {
	// 	WBF_STATE_INITIAL = 0,
	// 	WBF_STATE_FIRST_BAND,
	// 	WBF_STATE_SECOND_BAND,
	// 	WBF_STATE_THIRD_BAND
	// } WaveBandFinder_State;

	enum WaveBandFinder_Mode{
		WBF_MODE_WATERSHED,
		WBF_MODE_THRESHOLD
	};

	WaveBandFinder() : ImageFilter() {
		// state = WBF_STATE_INITIAL;
		mode = WBF_MODE_WATERSHED;
	};
	WaveBandFinder(WaveBandFinder_Mode m) : ImageFilter() {
		// state = WBF_STATE_INITIAL;
		mode = m;
	};
	void setMode(WaveBandFinder_Mode);
	void filter();
	void setThresholdedImage(cv::Mat* m);
private:
	// WaveBandFinder_State state;

	WaveBandFinder_Mode mode;

	int compCount;
	vector<cv::Mat> segments;
	cv::Mat thresholdedImage;

	void findMarkers(cv::Mat& thresholdedImage, cv::Mat& markerMask);
	void drawMarkers(cv::Mat& markerMask, cv::Mat& marker);
	void watershed(cv::Mat& filteredImage, cv::Mat& marker);
	void createSegments(cv::Mat& marker);

	void findByWatershed();
	void findByThreshold();

};

}

#endif