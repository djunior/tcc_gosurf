#include "iostream"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include "downsampledTimestack.h"

using namespace std;
using namespace cv;

namespace tcc {

DownsampledTimestack::DownsampledTimestack(VideoCapture& cap, int df) : Timestack(cap) {
	originalSampleRate = (int) cap.get(CV_CAP_PROP_FPS);
	downsampleFactor = df;
	cout << "DownsampleFactor: " << downsampleFactor << endl;
	frameCount = 0;
}

void DownsampledTimestack::setDownsampleFactor(int df) {
	cout << "setDownsampleFactor" << endl;
	downsampleFactor = df;
	sampleRate = originalSampleRate / downsampleFactor;
}

int DownsampledTimestack::getDownsampleFactor() {
	return downsampleFactor;
}

void DownsampledTimestack::setSampleRate(int sr) {
	cout << "setSampleRate" << endl;
	sampleRate = sr;
	downsampleFactor = originalSampleRate / sampleRate;
}

int DownsampledTimestack::getSampleRate() {
	return sampleRate;
}

void DownsampledTimestack::process(Mat& mat) {
	cout << "frameCount: " << frameCount << endl;
	cout << "DownsampleFactor: " << downsampleFactor << endl;
	if (frameCount == 0) {
		Timestack::process(mat);
	}
	frameCount++;
	if (frameCount == downsampleFactor)
		frameCount = 0;
}

}