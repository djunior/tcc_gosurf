/*
Copyright (c) 2014, Nghia Ho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <cassert>
#include <cmath>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

#include "stabilize.h"

namespace tcc {

using namespace std;
using namespace cv;

// This video stablisation smooths the global trajectory using a sliding average window

const int SMOOTHING_RADIUS = 80; // In frames. The larger the more stable the video, but less reactive to sudden panning
const int HORIZONTAL_BORDER_CROP = 20; // In pixels. Crops the border to reduce the black borders from stabilisation being too noticeable.

// 1. Get previous to current frame transformation (dx, dy, da) for all frames
// 2. Accumulate the transformations to get the image trajectory
// 3. Smooth out the trajectory using an averaging window
// 4. Generate new set of previous to current transform, such that the trajectory ends up being the same as the smoothed trajectory
// 5. Apply the new transformation to the video

struct TransformParam
{
    TransformParam() {}
    TransformParam(double _dx, double _dy, double _da) {
        dx = _dx;
        dy = _dy;
        da = _da;
    }

    double dx;
    double dy;
    double da; // angle
};

struct Trajectory
{
    Trajectory() {}
    Trajectory(double _x, double _y, double _a) {
        x = _x;
        y = _y;
        a = _a;
    }

    double x;
    double y;
    double a; // angle
};

class StableVideoViewer : public ImageProcessor {
private:
        enum StableVideoViewer_State {
            WAITING_FOR_ORIGINAL,
            WAITING_FOR_STABLE
        };

        Mat original,stable;
        StableVideoViewer_State state;

        void postProcess() {
            int vert_border = HORIZONTAL_BORDER_CROP * original.rows / original.cols; // get the aspect ratio correct

            Mat s(stable,Range(vert_border, stable.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, stable.cols-HORIZONTAL_BORDER_CROP));

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

            waitKey(1);
        }
public:
    void process(Mat& mat) {
        switch (state) {
            case WAITING_FOR_ORIGINAL:
                original = mat.clone();
                state = WAITING_FOR_STABLE;
                break;
            case WAITING_FOR_STABLE:
                stable = mat.clone();
                state = WAITING_FOR_ORIGINAL;
                postProcess();
                break;
        }
    }
};

void stabilize(VideoCapture &cap) {
    StableVideoViewer videoViewer;
    stabilize(cap,&videoViewer,&videoViewer);
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

void stabilize(VideoCapture &cap, ImageProcessor* origProc, ImageProcessor* stableProc)
{
    // For further analysis
    // ofstream out_transform("prev_to_cur_transformation.txt");
    // ofstream out_trajectory("trajectory.txt");
    // ofstream out_smoothed_trajectory("smoothed_trajectory.txt");
    // ofstream out_new_transform("new_prev_to_cur_transformation.txt");

    assert(cap.isOpened());

    Mat cur;
    Mat prev, prev_grey;

    cap >> prev;
    cvtColor(prev, prev_grey, COLOR_BGR2GRAY);

    // Step 1 - Get previous to current frame transformation (dx, dy, da) for all frames
    vector <TransformParam> prev_to_cur_transform; // previous to current

    int k=1;
    int max_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    Mat last_T;

    bool printed0 = false;
    bool printed25 = false;
    bool printed50 = false;
    bool printed75 = false;
    bool printed90 = false;
    bool printed99 = false;

    while(true) {
        cap >> cur;

        if(cur.data == NULL) {
            break;
        }

        // cvtColor(cur, cur_grey, COLOR_BGR2GRAY);
        Mat cur_grey(cur.size(),CV_8UC1);
        convertToGreyscale(cur, cur_grey);

        // vector from prev to cur
        vector <Point2f> prev_corner, cur_corner;
        vector <Point2f> prev_corner2, cur_corner2;
        vector <uchar> status;
        vector <float> err;

        goodFeaturesToTrack(prev_grey, prev_corner, 200, 0.01, 30);
        calcOpticalFlowPyrLK(prev_grey, cur_grey, prev_corner, cur_corner, status, err);

        // weed out bad matches
        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                prev_corner2.push_back(prev_corner[i]);
                cur_corner2.push_back(cur_corner[i]);
            }
        }

        // translation + rotation only
        Mat T = estimateRigidTransform(prev_corner2, cur_corner2, false); // false = rigid transform, no scaling/shearing

        // in rare cases no transform is found. We'll just use the last known good transform.
        if(T.data == NULL) {
            last_T.copyTo(T);
        }

        T.copyTo(last_T);

        // decompose T
        double dx = T.at<double>(0,2);
        double dy = T.at<double>(1,2);
        double da = atan2(T.at<double>(1,0), T.at<double>(0,0));

        prev_to_cur_transform.push_back(TransformParam(dx, dy, da));

        // out_transform << k << " " << dx << " " << dy << " " << da << endl;

        cur.copyTo(prev);
        cur_grey.copyTo(prev_grey);

        int percent = k * 100 / max_frames;
        // cout << "Percent " << percent << endl;
        switch(percent) {
            case 0:
                // cout << "Case 0" << endl;
                if (!printed0) {
                    printed0 = true;
                    cout << "Stabilization step 1: 0%...";
                }
                break;
            case 25:
                if (!printed25) {
                    printed25 = true;
                    cout << "25%...";
                }
                break;
            case 50:
                if (!printed50) {
                    printed50 = true;
                    cout << "50%...";
                }
                break;
            case 75:
                if (!printed75) {
                    printed75 = true;
                    cout << "75%...";
                }
                break;
            case 90:
                if (!printed90) {
                    printed90 = true;
                    cout << "90%...";
                }
                break;
            case 98:
            case 99:
                if (!printed99) {
                    printed99 = true;
                    cout << "99% completed!" << endl;;
                }
                break;
            default:
                break;
        }

        k++;
    }

    // Step 2 - Accumulate the transformations to get the image trajectory

    // Accumulated frame to frame transform
    double a = 0;
    double x = 0;
    double y = 0;

    vector <Trajectory> trajectory; // trajectory at all frames

    for(size_t i=0; i < prev_to_cur_transform.size(); i++) {
        x += prev_to_cur_transform[i].dx;
        y += prev_to_cur_transform[i].dy;
        a += prev_to_cur_transform[i].da;

        trajectory.push_back(Trajectory(x,y,a));

        // out_trajectory << (i+1) << " " << x << " " << y << " " << a << endl;
    }

    // Step 3 - Smooth out the trajectory using an averaging window
    vector <Trajectory> smoothed_trajectory; // trajectory at all frames

    for(size_t i=0; i < trajectory.size(); i++) {
        double sum_x = 0;
        double sum_y = 0;
        double sum_a = 0;
        int count = 0;

        for(int j=-SMOOTHING_RADIUS; j <= SMOOTHING_RADIUS; j++) {
            if(i+j >= 0 && i+j < trajectory.size()) {
                sum_x += trajectory[i+j].x;
                sum_y += trajectory[i+j].y;
                sum_a += trajectory[i+j].a;

                count++;
            }
        }

        double avg_a = sum_a / count;
        double avg_x = sum_x / count;
        double avg_y = sum_y / count;

        smoothed_trajectory.push_back(Trajectory(avg_x, avg_y, avg_a));

        // out_smoothed_trajectory << (i+1) << " " << avg_x << " " << avg_y << " " << avg_a << endl;
    }

    // Step 4 - Generate new set of previous to current transform, such that the trajectory ends up being the same as the smoothed trajectory
    vector <TransformParam> new_prev_to_cur_transform;

    // Accumulated frame to frame transform
    a = 0;
    x = 0;
    y = 0;

    for(size_t i=0; i < prev_to_cur_transform.size(); i++) {
        x += prev_to_cur_transform[i].dx;
        y += prev_to_cur_transform[i].dy;
        a += prev_to_cur_transform[i].da;

        // target - current
        double diff_x = smoothed_trajectory[i].x - x;
        double diff_y = smoothed_trajectory[i].y - y;
        double diff_a = smoothed_trajectory[i].a - a;

        double dx = prev_to_cur_transform[i].dx + diff_x;
        double dy = prev_to_cur_transform[i].dy + diff_y;
        double da = prev_to_cur_transform[i].da + diff_a;

        new_prev_to_cur_transform.push_back(TransformParam(dx, dy, da));

        // out_new_transform << (i+1) << " " << dx << " " << dy << " " << da << endl;
    }

    // Step 5 - Apply the new transformation to the video
    cap.set(CV_CAP_PROP_POS_FRAMES, 0);
    Mat T(2,3,CV_64F);

    k=0;

    while(k < max_frames-1) { // don't process the very last frame, no valid transform
        cap >> cur;

        if(cur.data == NULL) {
            break;
        }

        T.at<double>(0,0) = cos(new_prev_to_cur_transform[k].da);
        T.at<double>(0,1) = -sin(new_prev_to_cur_transform[k].da);
        T.at<double>(1,0) = sin(new_prev_to_cur_transform[k].da);
        T.at<double>(1,1) = cos(new_prev_to_cur_transform[k].da);

        T.at<double>(0,2) = new_prev_to_cur_transform[k].dx;
        T.at<double>(1,2) = new_prev_to_cur_transform[k].dy;

        Mat cur2,originalGreyMat,stableGreyMat,originalTransposedMat,stableTransposedMat;

        if (origProc != NULL && origProc != 0)
       	    origProc->process(cur);

        warpAffine(cur, cur2, T, cur.size());

        if (stableProc != NULL && stableProc != 0)
            stableProc->process(cur2);

        // cur2 = cur2(Range(vert_border, cur2.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, cur2.cols-HORIZONTAL_BORDER_CROP));

        // // Resize cur2 back to cur size, for better side by side comparison
        // resize(cur2, cur2, cur.size());

        // // Now draw the original and stablised side by side for coolness
        // Mat canvas = Mat::zeros(cur.rows, cur.cols*2+10, cur.type());

        // cur.copyTo(canvas(Range::all(), Range(0, cur2.cols)));
        // cur2.copyTo(canvas(Range::all(), Range(cur2.cols+10, cur2.cols*2+10)));

        // // If too big to fit on the screen, then scale it down by 2, hopefully it'll fit :)
        // if(canvas.cols > 1920) {
        //     resize(canvas, canvas, Size(canvas.cols/2, canvas.rows/2));
        // }

        // imshow("before and after", canvas);

        //char str[256];
        //sprintf(str, "images/%08d.jpg", k);
        //imwrite(str, canvas);

        // waitKey(20);

        k++;
    }
}

}
