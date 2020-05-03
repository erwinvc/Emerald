#pragma once
#include "camera.h"

class FirstPersonCam : public Camera {
private:
public:
	FirstPersonCam(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : Camera(viewportSize, fov, nearPlane, farPlane){}
	float movementSpeed = 4;
	float cameraHeight = 0.9f;

	void FixedUpdate() {}
	void Update(const TimeStep& time);

	void OnImGui() override {
		ImGui::InputFloat3("Position", (float*)&transform.m_position);
		ImGui::InputFloat3("Rotation", (float*)&transform.m_rotation);
		ImGui::InputFloat("Camera height", (float*)&cameraHeight);
	}

};