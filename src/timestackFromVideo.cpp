#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include "downsampledTimestack.h"
#include "gradedTimestack.h"
#include "timestack.h"
#include "processorPipeline.hpp"
#include "stabilize.h"

using namespace tcc;
using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	string path = argv[1];

 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	std::string video_extension = base_filename.substr(p,path.length());
	std::string image_extension = ".jpg";

	std::string outputOriginal = "output_images/" + file_without_extension + "_originalTimestack" + image_extension;
	std::string outputStable = "output_images/" + file_without_extension + "_stableTimestack" + image_extension;

	if (argc > 2) {
		outputOriginal = argv[2];
		outputOriginal += file_without_extension + "_originalTimestack" + image_extension;

		outputStable = argv[2];
		// outputStable += file_without_extension + "_stableTimestack" + image_extension;
	}

	VideoCapture cap(path.c_str());

	// GradedTimestack originalTimestack(cap);
	// GradedTimestack stableTimestack(cap);

	// DownsampledTimestack originalTimestack(cap,8);
	// DownsampledTimestack stableTimestack(cap,8);

	Timestack originalTimestack(cap);

	// Timestack stableTimestack0(cap);
	// stableTimestack0.setHorizontalPercent(0);
	// Timestack stableTimestack25(cap);
	// stableTimestack25.setHorizontalPercent(25);
	Timestack stableTimestack50(cap);
	stableTimestack50.setHorizontalPercent(50);
	// Timestack stableTimestack75(cap);
	// stableTimestack75.setHorizontalPercent(75);
	// Timestack stableTimestack100(cap);
	// stableTimestack100.setHorizontalPercent(100);

	// ProcessorPipeline stablePipeline;
	// stablePipeline.add(&stableTimestack0);
	// stablePipeline.add(&stableTimestack25);
	// stablePipeline.add(&stableTimestack50);
	// stablePipeline.add(&stableTimestack75);
	// stablePipeline.add(&stableTimestack100);

	// int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));

	// Size S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	// videoWriter.open(videoOutputPath, ex, cap.get(CV_CAP_PROP_FPS), S, true);
	
	stabilize(cap,&originalTimestack,&stableTimestack50);

	originalTimestack.save(outputOriginal);

	// stableTimestack0.save(outputStable + "timestack0/" + file_without_extension + "_stableTimestack.jpg");
	// stableTimestack25.save(outputStable + "timestack25/" + file_without_extension + "_stableTimestack.jpg");
	stableTimestack50.save(outputStable + "timestack50/" + file_without_extension + "_stableTimestack.jpg");
	// stableTimestack75.save(outputStable + "timestack75/" + file_without_extension + "_stableTimestack.jpg");
	// stableTimestack100.save(outputStable + "timestack100/" + file_without_extension + "_stableTimestack.jpg");

	// Mat* timestack_stable = stableTimestack.getTimestack();

	// std::string::size_type const p2(outputStable.find_last_of('.'));
	// std::string stable_without_extension = outputStable.substr(0, p2);

	return 0;
}