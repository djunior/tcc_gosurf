#ifndef __FILTER_PIPELINE_H__
#define __FILTER_PIPELINE_H__

#include <iostream>
#include "opencv/cv.h"
#include "imageFilter.h"

namespace tcc {

using namespace std;

class FilterPipeline : public ImageFilter {
private:
	vector<ImageFilter*> pipeline;
	cv::Mat pipelineOutputMat;
public:
	FilterPipeline() {};
	FilterPipeline(cv::Mat &m) : ImageFilter(m) {};
	FilterPipeline(cv::Mat *m) : ImageFilter(m) {};
	~FilterPipeline() {
		for (int i = 0; i < pipeline.size(); i++ ){
			delete pipeline[i];
		}
	};
	ImageFilter* addFilter(ImageFilter*);
	cv::Mat* getOutputMat();
	void filter();
	ImageFilter* getFilter(int);
};

}

#endif