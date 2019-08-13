#include "stdafx.h"

void Camera::UpdateProjectionMatrix() {
	m_projectionMatrix = Matrix4::Perspective(m_FOV, GetApplication()->GetAspect(), m_nearPlane, m_farPlane);
}