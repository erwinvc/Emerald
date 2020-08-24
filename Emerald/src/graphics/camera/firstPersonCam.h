#pragma once
#include "camera.h"

class Chunk;
class FirstPersonCam : public Camera {
private:
	bool m_isOnGround = false;
	float m_targetFOV = 90.0f;
	float m_stepHeight = 0.6f;

	float CollideX(glm::vec3& vel, const glm::vec3& offset);
	float CollideY(glm::vec3& vel, const glm::vec3& offset);
	float CollideZ(glm::vec3& vel, const glm::vec3& offset);
	void ResetPositionToBB(const AABB& axisalignedbb);


public:
	FirstPersonCam(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : Camera(viewportSize, fov, nearPlane, farPlane) {}
	void DrawUpdate(float partialTicks) override;
	void Update() override;
};