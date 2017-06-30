#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include "downsampledTimestack.h"
#include "gradedTimestack.h"
#include "timestack.h"
#include "stabilize.h"
#include "laplacianFilter.h"
#include "cannyFilter.h"
#include "waveDetector.h"

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
using namespace tcc;

string fileWithoutExtension(string path) {
 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	return file_without_extension;
}

CannyFilter* imgFilter;
int lowThreshold = 100;
string window_name = "Window";
Mat dst;

void CannyThreshold(int,void*) {
	imgFilter->setLowThreshold(lowThreshold);
	imgFilter->filter();
	dst = Scalar::all(0);
	imgFilter->getSourceMat()->copyTo(dst,*imgFilter->getFilteredImage());
	imshow(window_name,dst);
}

void filterImageInteractive(int argc, char* argv[]) {
	/// Load an image
	std::string path = argv[1];
	Mat src = imread( path );
	dst.create( src.size(), src.type() );

	if( !src.data )
	{ 
		return; 
	}

	imgFilter = new CannyFilter(src);

	int const max_lowThreshold = 100;

	namedWindow("threshold_window");
	namedWindow(window_name);

	/// Create a Trackbar for user to enter threshold
	createTrackbar( "Min Threshold:", "threshold_window", &lowThreshold, max_lowThreshold, CannyThreshold );
	CannyThreshold(0,0);
	// imgFilter.save("output_images/" + fileWithoutExtension(path) + "_filteredTimestack.jpg");

	waitKey(0);
}

void CannyFilterAux(CannyFilter& filter, int t, string& path) {
	filter.setLowThreshold(t);	
	filter.filter();
	stringstream ss;
	char str[256];
	sprintf(str,"%s_canny%d.jpg",path.c_str(),t);
	string fullPath = str;
	filter.save(fullPath);
}

void filterImage(string path) {
	// string path = argv[1];

 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	std::string path_without_extension = "output_images/" + file_without_extension;
	std::string canny_path = "output_images/canny/" + file_without_extension;

	Mat image = imread(path);

	LaplacianFilter laplacian(image);

	laplacian.filter();
	laplacian.save(path_without_extension + "_laplacian.jpg");

	CannyFilter canny(image);
	// canny.setLowThreshold(100);

	// for (int i = 0; i <= 120; i++) {
		CannyFilterAux(canny,0,path_without_extension);
		CannyFilterAux(canny,50,path_without_extension);
		CannyFilterAux(canny,90,path_without_extension);
		CannyFilterAux(canny,100,path_without_extension);
	// }
}

void timestackFromVideo(string path) {
	// if (argc < 2) {
	// 	std::cerr << "timestackFromVideo - Esperado o nome do video!" << std::endl;
	// 	return;
	// }

	// std::string path = argv[1];

 	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	std::string video_extension = base_filename.substr(p,path.length());
	std::string image_extension = ".jpg";

	std::string outputOriginal = "output_images/" + file_without_extension + "_originalTimestack" + image_extension;
	std::string outputStable = "output_images/" + file_without_extension + "_stableTimestack" + image_extension;

	VideoCapture cap(path.c_str());

	// GradedTimestack originalTimestack(cap);
	// GradedTimestack stableTimestack(cap);

	DownsampledTimestack originalTimestack(cap,8);
	DownsampledTimestack stableTimestack(cap,8);

	// int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));

	// Size S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	// videoWriter.open(videoOutputPath, ex, cap.get(CV_CAP_PROP_FPS), S, true);
	
	stabilize(cap,&originalTimestack,&stableTimestack);

	originalTimestack.save(outputOriginal);
	stableTimestack.save(outputStable);

	Mat* timestack_stable = stableTimestack.getTimestack();

	std::string::size_type const p2(outputStable.find_last_of('.'));
	std::string stable_without_extension = outputStable.substr(0, p2);

}

void stabilizeVideo(string path) {
	// if (argc < 2) {
	// 	std::cerr << "stabilizeVideo - Esperado o nome do video!" << std::endl;
	// 	return;
	// }

	// std::string path = argv[1];

	VideoCapture cap(path.c_str());

	stabilize(cap);
}

void detectReference(int argc, char* argv[]) {

	if (argc < 2) {
		std::cerr << "detectReference - Esperado o nome da imagem!" << std::endl;
		return;
	}

	int iLowH = 133;
	int iHighH = 162;

	int iLowS = 52; 
	int iHighS = 104;

	int iLowV = 73;
	int iHighV = 141;

	std::cout << "Inicio do main!" << std::endl;
	std::cout << "argc: " << argc << std::endl;

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

		Size size(480,720);//the dst image size,e.g.100x100
		Mat rThreshold, rOriginal;//dst image
		resize(imgThresholded,rThreshold,size);
		resize(imageMat,rOriginal,size);

		imshow("thresholded",rThreshold);
		imshow("original",rOriginal);

		// resizeWindow("image", 1280, 1280);

		waitKey(0);
    }
}

void applyFilterToVideo(string videoPath, string filterPath) {
	// string videoPath = argv[1];
	// string filterPath = argv[2];

	VideoCapture cap(videoPath);
	Mat filter = imread(filterPath);

	cout << "Filter rows " << filter.rows << " cols " << filter.cols << endl;

	Mat cur;
	int frame = 0;

	while(true) {
        cap >> cur;

        // cout << "Cur rows " << cur.rows << " cols " << cur.cols << endl;
        // cout << "Cur.row(middle) rows " << cur.col(cur.cols/2).rows << " cols " << cur.col09(cur.cols/2).cols << endl;
        if(cur.data == NULL) {
            break;
        }

        Mat timestackFrame = filter.col(frame);

        // cout << "filter cols " << filter.cols << " rows " << filter.rows << endl;

        cvtColor(timestackFrame, timestackFrame, CV_BGR2GRAY);

        for (int i = 0; i < timestackFrame.rows; i++) {
			// cout << "Timestack frame value " << (int) timestackFrame.at<uchar>(i) << endl; 
			int value = (int) timestackFrame.at<uchar>(i);
			if (value > 0) {
				// cout << "Changing pixel at point "
				// cur.col(cur.cols/2).at<Scalar>(i) = Scalar(255,0,0);
				line(cur,Point(cur.cols/2,i),Point(cur.cols/2,i),Scalar(0,0,255));
			}       	
        }

        resize(cur,cur,Size(cur.cols/2,cur.rows/2));
        imshow("video",cur);
        waitKey(20);
        frame++;
    }
}

void processTimestack(string path) {
	Mat timestack = imread(path);
	cvtColor(timestack, timestack, CV_BGR2GRAY);
	WaveDetector wd;
	wd.process(timestack);
}

void readString(string label, string& str) {
	cout << label << endl;
	cin >> str;
}

int main(int argc, char* argv[]) {
	int option;
	string path;
	string path2;

	if (argc <= 1) {
		cout << "Selecione a opção desejada:" << endl;
		cout << "1. applyFilter to video" << endl;
		cout << "2. Filter image" << endl;
		cout << "3. Create timestack from video" << endl;
		cout << "4. Process a timestack" << endl;
		// cout << "4. stabilize video" << endl;

		cin >> option;
	} else {
		option = atoi(argv[2]);
	}

	switch(option) {
		case 1:
			readString("Digite o path para o video:",path);
			readString("Digite o path para o filtro:",path2);
			applyFilterToVideo(path,path2);
			break;
		case 2:
			if (argc <= 1)
				readString("Digite o path para a imagem:",path);
			else
				path.assign(argv[1]);
			filterImage(path);
			break;
		case 3:
			readString("Digite o path para o video:",path);
			timestackFromVideo(path);
			break;
		case 4:
			if (argc <= 1)
				readString("Digite o path para o timestack:",path);
			else {
				path.assign(argv[1]);
			}

			processTimestack(path);
			break;
	}
	// applyFilterToVideo(argc,argv);
	// filterImage(argc,argv);
	// timestackFromVideo(argc,argv);
	// stabilizeVideo(argc,argv);
	return 0;
}
