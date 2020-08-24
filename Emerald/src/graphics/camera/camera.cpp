#include "stdafx.h"

Camera* Camera::active = nullptr;
Camera* Camera::uiActive = nullptr;

Camera::Camera(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : m_viewPort(glm::vec4(0, 0, viewportSize.x, viewportSize.y)) {
	SetProjectionMatrix(fov, nearPlane, farPlane); UpdateViewMatrix(0);
}


void Camera::UpdateViewMatrix(float partialTicks) {
	glm::vec3 d = lastUpdateTransform.position + (transform.position - lastUpdateTransform.position) * partialTicks;
	//glm::vec3 dr = lastUpdateTransform.rotation + (transform.rotation - lastUpdateTransform.rotation) * partialTicks;
	//m_viewMatrix = glm::translate(glm::mat4(1.0f), d) * glm::toMat4(glm::quat(glm::vec3(-dr.x, -dr.y, -dr.z)));

	m_viewMatrix = glm::translate(glm::mat4(1.0f), d) * glm::toMat4(transform.GetOrientation());
	m_inverseViewMatrix = m_viewMatrix;
	m_viewMatrix = glm::inverse(m_viewMatrix);
}

void Camera::DrawUpdate(float partialTicks) {
	UpdateViewMatrix(partialTicks);
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

BlockSide Camera::GetFacingDirection() {
	float rot = transform.rotation.y;
	if (Math::Within(rot, Math::QUARTER_PI, Math::HALF_PI + Math::QUARTER_PI)) return BlockSide::EAST;
	else if (Math::Within(rot, Math::HALF_PI + Math::QUARTER_PI, Math::PI + Math::QUARTER_PI)) return BlockSide::SOUTH;
	else if (Math::Within(rot, Math::PI + Math::QUARTER_PI, Math::TWO_PI - Math::QUARTER_PI)) return BlockSide::WEST;
	return BlockSide::NORTH;
}


void Camera::OnImGui() {
	ImGui::Text(Format_t("Direction: %s", BlockSideToString(GetFacingDirection()).c_str()));

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