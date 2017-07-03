#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

using namespace std;
using namespace cv;

double intensity(Mat& m, int row_index) {
	cout << "Calculating intensity from row " << row_index << endl;
	double result;
	Mat row = m.row(row_index);

	cout << "Row size: " << row.cols << "," << row.rows << endl;

	for (int i = 0; i < row.cols; i++)
		result+= (m.at<uchar>(i) / 255);

	return result;
}

double f1(Mat& m, int row_index) {
	cout << "Calculating f1 from row " << row_index << endl;
	double max_intensity = 0;
	for (int i = 0; i < m.rows; i++) {
		double tmp_intensity = intensity(m,i);
		if (tmp_intensity > max_intensity)
			max_intensity = tmp_intensity;
	}
	double row_intensity = intensity(m,row_index);
	double result = row_intensity / max_intensity;
	cout << "Row Intensity: " << row_intensity << endl;
	cout << "Max Intensity: " << max_intensity << endl;
	return result;
}

int main(int argc, char** argv) {
	string path = argv[1];
	Mat image = imread(path);

	cout << "Image size: " << image.cols << "," << image.rows << endl;

	Mat greyImage;
	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	double f = f1(greyImage,460);

	cout << "float value " << f << endl;

	return 0;
}