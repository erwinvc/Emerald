#pragma once
#include "camera.h"

class Chunk;
class FirstPersonCam : public Camera {
private:
	bool m_isOnGround = false;
	//glm::vec3 m_targetRotation = glm::vec3(0, 0, 0);
	glm::vec3 m_acceleration = glm::vec3(0, 0, 0);
	bool m_isFlying = true;
	bool m_grabMouse = false;
public:
	glm::vec3 m_velocity = glm::vec3(0, 0, 0);
	FirstPersonCam(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : Camera(viewportSize, fov, nearPlane, farPlane) {}
	void Collide(const glm::vec3& vel);
	void FixedUpdate() {}
	void Update(const TimeStep& time);
	void DrawUpdate();

	void OnImGui() override {
		ImGui::InputFloat3("Position", (float*)&transform.position);
		ImGui::InputFloat3("Rotation", (float*)&transform.rotation);
		if (ImGui::InputFloat("Near", &m_nearPlane, 0.000001f, 10000.0f)) {
			UpdateProjectionMatrix();
		}

		if (ImGui::InputFloat("Far", &m_farPlane, 0.000001f, 10000.0f)) {
			UpdateProjectionMatrix();
		}

		if (ImGui::SliderFloat("FOV", &m_FOV, 0, 180)) {
			UpdateProjectionMatrix();
		}
	}

};