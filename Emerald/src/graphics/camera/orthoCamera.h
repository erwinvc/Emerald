#pragma once
#include "camera.h"

class OrthoCamera : public Camera {
protected:
	void UpdateProjectionMatrix() override;
	
public:
	OrthoCamera() : Camera(glm::vec2(1.0f, 1.0f), 0.0f, 0.0f, 0.0f) {}
};