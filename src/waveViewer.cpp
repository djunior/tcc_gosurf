#include <iostream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <fstream>

#include "trajectory.h"

using namespace std;
using namespace cv;
using namespace tcc;

int main(int argc, char** argv) {

	vector< pair<Trajectory::Point,Trajectory::Point> > waves;

	VideoCapture cap(argv[1]);


	string line_bottom;
	string line_top;
	ifstream f("waves.txt");
	if (f.is_open())
	{
		while ( ! f.eof() )
	    {
	      f >> line_bottom;
	      f >> line_top;

	      cout << "line bottom: " << line_bottom << endl;
	      cout << "line top: " << line_top << endl;

	      int bottom_x,bottom_y;
	      int top_x,top_y;

	      sscanf(line_bottom.c_str(),"%i,%i",&bottom_x,&bottom_y);
	      sscanf(line_top.c_str(),"%i,%i",&top_x,&top_y);

	      Trajectory::Point bottom(bottom_x,bottom_y);
	      Trajectory::Point top(top_x,top_y);

	      pair<Trajectory::Point,Trajectory::Point> wave(bottom,top);
	      waves.push_back(wave);
	    }
	    f.close();
	}

	Mat frame;
	int frame_count = 0;
	while(true) {
		cap >> frame;

		if(frame.data == NULL)
			break;

		// Mat frame;
    	// transpose(t_frame,frame);

		for (int i = 0; i < waves.size(); i++) {
			Trajectory::Point bottom = waves[i].first;
			Trajectory::Point top = waves[i].second;

			// float a = (top.x - bottom.x)/(top.y - bottom.y);
			// float b = top.x - a*top.y;

			if (frame_count >= bottom.y && frame_count <= top.y) {
				// int top_y = a*frame_count + b;

				Point bottom_p1(0,bottom.x);
				Point bottom_p2(frame.cols,bottom.x);

				line(frame,bottom_p1,bottom_p2,Scalar(0,0,255),2);

				Point top_p1(0,top.x);
				Point top_p2(frame.cols,top.x);

				line(frame,top_p1,top_p2,Scalar(0,0,255),2);
			}
		}

		imshow("frame",frame);

		frame_count++;

		waitKey(30);
	}	
}