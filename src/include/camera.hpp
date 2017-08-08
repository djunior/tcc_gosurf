#ifndef __CAMERA_H__
#define __CAMERA_H__

#define PI 3.14159265

#include "math.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace tcc {
	
class Camera {
private:
	
	double convertToRad(double ang) {
		return ang * PI / 180.0;
	}

	/*---calculateAngleBegin---*/
	// Função que calcula o angulo em radianos correspondente de um pixel na imagem.
	// PI é definido como 3.14159265
	double calculateAngle(int pixel) {
		double ang = cameraAngle - focalAngle/2 + ( (pixel * focalAngle) / imageSize );
		return ang * PI / 180.0;
	}
	/*---calculateAngleEnd---*/

	double convertToAng(double rad) {
		return rad * 180.0 / PI;
	}

	double calculateFocalAngle(double sensorSize, double focalLength) {
		return 2 * convertToAng(atan(sensorSize / (2 * focalLength)));
	}

public:
	int imageSize;
	double cameraAngle, focalAngle, cameraHeight;

	Camera(double cAngle, double fAngle, double cHeight, int iSize) :
		cameraAngle(cAngle),
		focalAngle(fAngle),
		cameraHeight(cHeight),
		imageSize(iSize) 
	{

	}

	Camera(double cAngle, double focalLength, double sensorSize, double cHeight, int iSize) : 
			cameraAngle(cAngle),
			imageSize(iSize),
			cameraHeight(cHeight) 
	{
		focalAngle = calculateFocalAngle(sensorSize,focalLength);
	}

	Camera():
		cameraAngle(0),
		imageSize(0),
		focalAngle(0),
		cameraHeight(0)
	{
		load("iphone_camera.txt");
	}

	Camera(std::string fileName):
		cameraAngle(0),
		imageSize(0),
		focalAngle(0),
		cameraHeight(0)
	{
		load(fileName);
	}

	/*---calculateRealHeightBegin---*/
	// Função que calcula a altura real em metros entre dois pixels
	double calculateRealHeight(int bottom, int top) {
		// Ajustando parametros bottom e top para a origem estar
		// no canto inferior esquerdo (modelo Browne de praia)
		// e não no canto superior esquerdo (padrão OpenCV)
		bottom = imageSize - bottom;
		top = imageSize - top;

		double angleBottom = calculateAngle(bottom);
		double angleTop = calculateAngle(top);

		return cameraHeight * ( 1 - ( tan(angleBottom) / tan(angleTop) ) );
	}
	/*---calculateRealHeightEnd---*/

	void load(std::string fileName) {
		std::ifstream f(fileName.c_str());
		std::string line;

		if (f.is_open())
		{
			double focalLength = 0;
			double sensorSize = 0;

			while ( ! f.eof() )
		    {
		    	f >> line;

		    	std::size_t eqPos = line.find("=");

		    	std::string param = line.substr(0,eqPos);
		    	std::string value = line.substr(eqPos+1,line.length());

		    	if (param == "cameraAngle")
	    			cameraAngle = std::stod(value);
		    	else if(param == "focalAngle")
	    			focalAngle = std::stod(value);
		    	else if (param == "cameraHeight")
		    		cameraHeight = std::stod(value);
				else if (param == "imageSize")
		    		imageSize = std::stoi(value);
				else if (param == "focalLength")
	    			focalLength = std::stod(value);
	    		else if (param == "sensorSize")
		    		sensorSize = std::stod(value);

		    }
		    f.close();

		    if (focalLength > 0 && sensorSize > 0 && focalAngle == 0) {
		    	focalAngle = calculateFocalAngle(sensorSize,focalLength);
		    }
		}
	}

	void save() {
		ofstream f("camera.txt");

		if (f.is_open())
		{
			f << "cameraAngle=" << cameraAngle << endl;
			f << "focalAngle=" << focalAngle << endl;
			f << "cameraHeight=" << cameraHeight << endl;
			f << "imageSize=" << imageSize << endl;
		    f.close();
		}
	}

};

}

#endif