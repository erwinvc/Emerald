#pragma once
#include "camera.h"

class Chunk;
class FirstPersonCam : public Camera {
private:
	bool m_isOnGround = false;
	float m_targetFOV = 90.0f;
	
	void Collide(const glm::vec3& vel);

public:
	FirstPersonCam(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : Camera(viewportSize, fov, nearPlane, farPlane) {}
	
	void Update() override;

	void OnImGui() override;
};