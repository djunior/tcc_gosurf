#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"

/*
	Image info:
	size: 2448 x 3264 px

	reference :
	color: 106 31 39
	size: 1960 px - 14 cm
	distance: 21 cm

	object info:
	color: 108 75 102 255
	height: 358 px - 7.6 cm
	distance: 62.7 cm
*/

int main(int argc, char* argv[]) {
	std::cout << "Inicio do main!" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	if (argc < 2) {
		std::cerr << "Esperado o nome da imagem!" << std::endl;
		return 1;
	}
	
	std::string image_path(argv[1]);

	cv::Mat imageMat = cv::imread(image_path,CV_LOAD_IMAGE_COLOR);

	std::cout << "mat width: " << imageMat.rows << " mat height: " << imageMat.cols << std::endl;

	return 0;
}