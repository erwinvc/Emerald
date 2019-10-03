#pragma once
#include "camera.h"

class CarCam : public Camera
{
private:
	float CameraHeight = 0;

public:
	CarCam();
};