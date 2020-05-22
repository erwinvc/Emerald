#include "stdafx.h"
#include "carCam.h"

CarCam::CarCam() : Camera(glm::vec2(1.0f, 1.0f), 70, 0.1f, 1000.0f) {
	transform.rotation.x = 0.1f * Math::PI;
}