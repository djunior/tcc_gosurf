#ifndef __PROCESSOR_PIPELINE_H__
#define __PROCESSOR_PIPELINE_H__

#include "opencv/cv.h"
#include "imageProcessor.h"

namespace tcc {

using namespace std;

class ProcessorPipeline : public ImageProcessor {
private:
	vector<ImageProcessor*> pipeline;
public:
	ProcessorPipeline() {};
	~ProcessorPipeline() {
		// for (int i = 0; i < pipeline.size(); i++ ){
		// 	delete pipeline[i];
		// }
		pipeline.clear();
	};
	void add(ImageProcessor* ip) {
		pipeline.push_back(ip);
	};
	void process(cv::Mat& m) {
		for (int i = 0; i < pipeline.size(); i++) {
			ImageProcessor* imgProcessor = pipeline[i];
			imgProcessor->process(m);
		}
	};
};

}

#endif