#pragma once
#include "camera.h"

class FirstPersonCam : public Camera {
private:
public:
	FirstPersonCam(float fov, float nearPlane, float farPlane) : Camera(fov, nearPlane, farPlane) {}
	float movementSpeed = 4;
	float cameraHeight = 0.9f;

	void FixedUpdate() {}
	void Update(const TimeStep& time);

	void OnImGui() override {
		ImGui::InputFloat3("Position", (float*)&m_position);
		ImGui::InputFloat3("Rotation", (float*)&m_rotation);
		ImGui::InputFloat("Camera height", (float*)&cameraHeight);
	}

};