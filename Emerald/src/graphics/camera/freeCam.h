#pragma once
#include "camera.h"

class FreeCam : public Camera {
private:
public:
	FreeCam(float fov, float nearPlane, float farPlane) : Camera(fov, nearPlane, farPlane) {}

    const float movementSpeed = 10.0f;

	void FixedUpdate() {}
	void Update(const TimeStep& time);

};