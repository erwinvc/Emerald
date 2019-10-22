#include "stdafx.h"

void Camera::UpdateProjectionMatrix() {
	float aspect = m_viewPort.z / m_viewPort.w;
	m_projectionMatrix = Matrix4::Perspective(m_FOV, aspect, m_nearPlane, m_farPlane);
}