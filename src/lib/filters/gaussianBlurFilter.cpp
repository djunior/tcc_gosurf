#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

#include "gaussianBlurFilter.h"

using namespace cv;

namespace tcc {

void GaussianBlurFilter::setKernelSize(int k) {
  kernel_size = k;
}

void GaussianBlurFilter::filter() {
  /// Reduce noise with a kernel N x N
  GaussianBlur( srcMat, filteredMat, Size(kernel_size,kernel_size), 0, 0, BORDER_DEFAULT );
}

}