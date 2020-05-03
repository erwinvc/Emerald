#pragma once
#include "camera.h"

class OrthoCamera : public Camera {
protected:
	void UpdateProjectionMatrix() override {
		m_projectionMatrix = glm::ortho(0.0f, m_viewPort.z, 0.0f, m_viewPort.w, -1.0f, 1.0f);
	}
	
public:
	OrthoCamera() : Camera(glm::vec2(1.0f, 1.0f), 0.0f, 0.0f, 0.0f) {}
};