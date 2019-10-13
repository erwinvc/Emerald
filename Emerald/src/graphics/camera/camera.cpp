#include "stdafx.h"

float Camera::offset = 0.1f;

void Camera::UpdateProjectionMatrix() {
	float aspect = m_viewPort.x / m_viewPort.y;
	m_projectionMatrix = Matrix4::Perspective(m_FOV, aspect, m_nearPlane, m_farPlane);
}