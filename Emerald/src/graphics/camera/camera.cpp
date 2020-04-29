#include "stdafx.h"

Camera* Camera::active = nullptr;
Camera* Camera::uiActive = nullptr;

void Camera::UpdateViewMatrix() {
	m_viewMatrix = glm::translate(glm::mat4(1.0f), transform.m_position) * glm::toMat4(transform.GetOrientation());
	m_inverseViewMatrix = m_viewMatrix;
	m_viewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::UpdateProjectionMatrix() {
	if (m_viewPort.w == 0) return;
	float aspect = m_viewPort.z / m_viewPort.w;
	m_projectionMatrix = glm::perspective(glm::radians(m_FOV), aspect, m_nearPlane, m_farPlane);
	m_inverseProjectionMatrix = glm::inverse(m_projectionMatrix);
}

void Camera::SetProjectionMatrix(float fov, float nearPlane, float farPlane) {
	m_FOV = fov;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	UpdateProjectionMatrix();
}

void Camera::SetViewport(uint x, uint y, uint width, uint height) {
	if (m_viewPort.x == x && m_viewPort.y == y && m_viewPort.z == width && m_viewPort.w == height) return;
	m_viewPort.x = (float)x;
	m_viewPort.y = (float)y;
	m_viewPort.z = (float)width;
	m_viewPort.w = (float)height;
	UpdateProjectionMatrix();
}

void Camera::OnImGui() {
	ImGui::InputFloat3("Position", (float*)&transform.m_position);
	ImGui::InputFloat3("Rotation", (float*)&transform.m_rotation);
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

CornerRayPositions Camera::GetCornerRays(float offset) {
	glm::vec3& ray1 = GroundRaycast::GetScreenPosition(glm::vec2(0.0f + offset, 0.0f + offset));
	glm::vec3& ray2 = GroundRaycast::GetScreenPosition(glm::vec2(1.0f - offset, 0.0f + offset));
	glm::vec3& ray3 = GroundRaycast::GetScreenPosition(glm::vec2(1.0f - offset, 1.0f - offset));
	glm::vec3& ray4 = GroundRaycast::GetScreenPosition(glm::vec2(0.0f + offset, 1.0f - offset));
	CornerRayPositions positions;

	positions.TL = GroundRaycast::GetGroundPosition(ray1, 0);
	positions.TR = GroundRaycast::GetGroundPosition(ray2, 0);
	positions.BR = GroundRaycast::GetGroundPosition(ray3, 0);
	positions.BL = GroundRaycast::GetGroundPosition(ray4, 0);
	return positions;
}