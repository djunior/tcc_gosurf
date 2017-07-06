#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

using namespace std;
using namespace cv;

long long intensity(Mat& m, int row_index) {
	// cout << "Calculating intensity from row " << row_index << endl;
	long long result = 0;
	Mat row = m.row(row_index);

	// cout << "Row size: " << row.cols << "," << row.rows << endl;

	for (int i = 0; i < row.cols; i++) {
		// cout << "Adding " << r << endl;
		result += row.at<uchar>(i);
	}
	// cout << "returning " << result << endl;

	return result;
}

double PRC(Mat& m, int row_index, int percentile) {
	Mat row = m.row(row_index);
	Mat sortedRow;

	cv::sort(row,sortedRow,CV_SORT_ASCENDING);

	int n = (sortedRow.cols * percentile) / 100;

	double value = (double) sortedRow.at<uchar>(n) / 255;

	return value;
}

double f1(Mat& m, int row_index) {
	// cout << "Calculating f1 from row " << row_index << endl;

	double max_intensity = 0;
	for (int i = 0; i < m.rows; i++) {
		double tmp_intensity = intensity(m,i);
		// cout << "Comparing " << tmp_intensity << " with " << max_intensity << endl;
		if (tmp_intensity > max_intensity) {
			// cout << "Found new max intensity: " << tmp_intensity << endl;
			max_intensity = tmp_intensity;
		}
	}

	long long row_intensity = intensity(m,row_index);
	double result = (double) (row_intensity / max_intensity);
	// cout << "Row Intensity: " << row_intensity << endl;
	// cout << "Max Intensity: " << max_intensity << endl;
	return result;
}

double f2(Mat& m, int row_index) {
	Mat row = m.row(row_index);

	double prc5 = PRC(m,row_index,5);
	double prc95 = PRC(m,row_index,95);

	// for (int i = 0; i < row.cols; i++) {

	// }

	return prc95 - prc5;
}

void detectBreakzone(Mat& m, bool allowsplit) {
	int v0 = -1;
	int v1 = -1;
	// int vM = -1;
	double threshold = 0.5;

	for (int i = 250; i < m.rows; i++) {
		double f = f1(m,i);
		if (f > threshold) {
			if (v0 == -1)
				v0 = i;

			v1 = i;
		}
	}

	cout << "first v0 = " << v0 << endl;
	cout << "firs v1 = " << v1 << endl;

	while(v0 < m.rows && f1(m,v0) > f1(m,v0+1) && f2(m,v0) > f2(m,v0 + 1)) 
		v0++;

	while(v1 >= 0 && f1(m,v1)/f2(m,v1) > 2)
		v1--;

	cout << "v0 = " << v0 << endl;
	cout << "v1 = " << v1 << endl;

	Mat breakzone = m.rowRange(v0,v1);
	imshow("breakzone",breakzone);
	waitKey(0);
}

int main(int argc, char** argv) {
	string path = argv[1];
	Mat image = imread(path);

	cout << "Image size: " << image.cols << "," << image.rows << endl;

	Mat greyImage;
	cvtColor(image,greyImage,COLOR_BGR2GRAY);

	equalizeHist(greyImage,greyImage);

	detectBreakzone(greyImage,false);

	// for (int i = 0; i < greyImage.rows; i++) {
	// 	double f = f1(greyImage,i);
	// 	cout << "f1(" << i << ") = " << f << endl;

	// 	cout << "5th percentile: " << PRC(greyImage,i,5) << endl;
	// 	cout << "95th percentile: " << PRC(greyImage,i,95) << endl;
	// }

	return 0;
}