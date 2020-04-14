#include "stdafx.h"

void Camera::UpdateProjectionMatrix() {
	if (m_viewPort.w == 0) return;
	float aspect = m_viewPort.z / m_viewPort.w;
	m_projectionMatrix = glm::perspective(glm::radians(m_FOV), aspect, m_nearPlane, m_farPlane);
}