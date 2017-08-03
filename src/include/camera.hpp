#ifndef __CAMERA_H__
#define __CAMERA_H__

#define PI 3.14159265

#include "math.h"

using namespace std;

namespace tcc {
	
class Camera {
private:

	double convertToRad(double ang) {
		return ang * PI / 180.0;
	}

	double convertToAng(double rad) {
		return rad * 180.0 / PI;
	}

	double calculateAngle(int pixel) {
		// cout << "CameraAngle: " << cameraAngle << endl;
		// cout << "Focal Angle: " << focalAngle/2 << endl;
		// cout << "Pixel angle: " << ( (pixel * focalAngle) / imageSize ) << endl;
		double ang = cameraAngle - focalAngle/2 + ( (pixel * focalAngle) / imageSize );
		// cout << "Calculating angle " << ang << endl;
		return convertToRad(ang);
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
		cameraAngle(83),
		cameraHeight(6.0),
		imageSize(720) 
	{
		focalAngle = calculateFocalAngle(3.6,4.15);
	}

	double calculateRealHeight(int bottom, int top) {
		bottom = imageSize - bottom;
		top = imageSize - top;
		// cout << "Calculating real height: " << bottom << " to " << top << endl;

		double angleBottom = calculateAngle(bottom);
		double angleTop = calculateAngle(top);

		// cout << "Angle bottom: " << angleBottom << endl;
		// cout << "Angle Top: " << angleTop << endl;

		// cout << "Camera Height: " << cameraHeight << endl;

		// cout << "Focal Angle: " << focalAngle << endl;

		// cout << "Image Size: " << imageSize << endl;

		// cout << "Camera Angle: " << cameraAngle << endl;

		// cout << "tan(angleTop): " << tan(angleTop) << endl;

		// cout << "tan(angleBottom): " << tan(angleBottom) << endl;

		// cout << "( tan(angleTop) / tan(angleBottom) )" << ( tan(angleBottom) / tan(angleTop) ) << endl;

		// cout << "1 - ( tan(angleTop) / tan(angleBottom) )" << 1 - ( tan(angleBottom) / tan(angleTop) ) << endl;

		return cameraHeight * ( 1 - ( tan(angleBottom) / tan(angleTop) ) );
	}
};

}

#endif