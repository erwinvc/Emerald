#pragma once
#include "camera.h"

class FreeCam : public Camera {
private:
public:
	const float movementSpeed = 10.0f;

	FreeCam(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : Camera(viewportSize, fov, nearPlane, farPlane) {}

	void FixedUpdate() {}
	void Update() override;
};