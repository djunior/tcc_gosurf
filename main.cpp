#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

/*
	Image info:

	Sensor size: 1.22um

	size: 2448 x 3264 px

	reference :
	color: 106 31 39
	size: 1960 px - 14 cm
	distance: 21 cm

	Low Hue: 133
	High Hue: 162
	Low saturation: 52
	High saturation: 104
	Low value: 73
	High value: 141

	object info:
	color: 108 75 102 255
	height: 358 px - 7.6 cm
	distance: 62.7 cm
*/

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {

	int iLowH = 133;
	int iHighH = 162;

	int iLowS = 52; 
	int iHighS = 104;

	int iLowV = 73;
	int iHighV = 141;

	std::cout << "Inicio do main!" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	if (argc < 2) {
		std::cerr << "Esperado o nome da imagem!" << std::endl;
		return 1;
	}

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
	
	std::string image_path(argv[1]);

	Mat hsvImage;
	cv::Mat imageMat = cv::imread(image_path,CV_LOAD_IMAGE_COLOR);

	Mat imgLines = Mat::zeros( imageMat.size(), CV_8UC3 );

	cvtColor(imageMat,hsvImage,CV_RGB2HSV);

	while(true) {

		cout << "Low Hue: " << iLowH << endl;
		cout << "High Hue: " << iHighH << endl;

		cout << "Low saturation: " << iLowS << endl;
		cout << "High saturation: " << iHighS << endl;

		cout << "Low value: " << iLowV << endl;
		cout << "High value: " << iHighV << endl;

  		Mat imgThresholded;

   		inRange(hsvImage, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		std::cout << "mat width: " << imageMat.rows << " mat height: " << imageMat.cols << std::endl;

		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

		//morphological closing (removes small holes from the foreground)
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		int iLastX = -1;
		int iLastY = -1;

		cout << "dM01: " << dM01 << endl;
		cout << "dM10: " << dM10 << endl;
		cout << "dArea = " << dArea << endl;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 10000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;        

			cout << "PosX: " << posX << endl;
			cout << "PosY: " << posY << endl;
			    
			//Draw a red line from the previous point to the current point
			Scalar color = Scalar( 255, 255, 255 );
			circle( imgLines, Point(posX, posY), 4, color, -1, 8, 0 );

			unsigned int half_width = 0;
			unsigned int half_height = 0;

			for (int i = posX; i > 0; i--) {
				Scalar colour = imgThresholded.at<uchar>(Point(i, posY));
				if(colour.val[0]==0) {
					half_width = posX - (i + 1);
					break;
				}
			}

			for (int j = posY; j > 0; j--) {
				Scalar colour = imgThresholded.at<uchar>(Point(posX, j));
				if(colour.val[0]==0) {
					half_height = posY - (j+1);
					break;
				}
			}

			cout << "Width: " << (half_width*2) << endl;
			cout << "Height: " << (half_height*2) << endl;

		}

	  	imageMat = imageMat + imgLines;

		Size size(1280,720);//the dst image size,e.g.100x100
		Mat rThreshold, rOriginal;//dst image
		resize(imgThresholded,rThreshold,size);
		resize(imageMat,rOriginal,size);

		imshow("thresholded",rThreshold);
		imshow("original",rOriginal);

		// resizeWindow("image", 1280, 1280);


		waitKey(0);
    }

	return 0;
}