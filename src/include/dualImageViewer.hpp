#ifndef __DUAL_IMAGE_VIEWER_H__
#define __DUAL_IMAGE_VIEWER_H__

#include "imageFilter.h"

namespace tcc {

using namespace cv;

const int SMOOTHING_RADIUS = 80; // In frames. The larger the more stable the video, but less reactive to sudden panning
const int HORIZONTAL_BORDER_CROP = 20; // In pixels. Crops the border to reduce the black borders from stabilisation being too noticeable.

class DualImageViewer : public ImageFilter {
private:
        Mat original;
public:
	void setOriginalMat(Mat& m) {
		original = m.clone();
	}
    void filter() {
        int vert_border = HORIZONTAL_BORDER_CROP * original.rows / original.cols; // get the aspect ratio correct

        Mat s(srcMat,Range(vert_border, srcMat.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, srcMat.cols-HORIZONTAL_BORDER_CROP));

        // Resize cur2 back to cur size, for better side by side comparison
        resize(s, s, original.size());

        // Now draw the original and stablised side by side for coolness
        Mat canvas = Mat::zeros(original.rows, original.cols*2+10, original.type());

        original.copyTo(canvas(Range::all(), Range(0, s.cols)));
        s.copyTo(canvas(Range::all(), Range(s.cols+10, s.cols*2+10)));

        // If too big to fit on the screen, then scale it down by 2, hopefully it'll fit :)
        if(canvas.cols > 1920) {
            resize(canvas, canvas, Size(canvas.cols/2, canvas.rows/2));
        }

        imshow("before and after", canvas);

        waitKey(0);
    }
};

}

#endif