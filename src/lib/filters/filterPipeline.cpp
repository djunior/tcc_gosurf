#include "filterPipeline.h"
#include "opencv/cv.h"

namespace tcc {

using namespace std;
using namespace cv;

ImageFilter* FilterPipeline::addFilter(ImageFilter *f) {
	pipeline.push_back(f);
	return f;
}

// Mat* FilterPipeline::getFilteredImage() {
// 	return &srcMat;
// }

void FilterPipeline::filter() {
	Mat* mat = &srcMat;
	filteredMat = mat->clone();

	for (int i = 0; i < pipeline.size(); i++) {
		ImageFilter* imgFilter = pipeline[i];
		imgFilter->setSourceMat(mat);
		imgFilter->filter();
		mat = imgFilter->getFilteredImage();
	}

	pipelineOutputMat = mat->clone();
}

Mat* FilterPipeline::getOutputMat() {
	return &pipelineOutputMat;
}

ImageFilter* FilterPipeline::getFilter(int i){
	return pipeline[i];
}

}