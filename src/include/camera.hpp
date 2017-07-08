#ifndef __CAMERA_H__
#define __CAMERA_H__

#define PI 3.14159265

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
		double ang = cameraAngle - focalAngle/2 + ( (pixel * focalAngle) / imageSize);
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
		focalAngle = calculateFocalAngle(24.0,29.0);
	}

	double calculateRealHeight(int bottom, int top) {
		double angleBottom = calculateAngle(bottom);
		double angleTop = calculateAngle(top);
		return cameraHeight * ( 1 - ( tan(angleTop) / tan(angleBottom) ) );
	}
};

}

#endif