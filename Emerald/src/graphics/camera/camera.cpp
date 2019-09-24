#include "stdafx.h"

void Camera::UpdateProjectionMatrix() {
	m_projectionMatrix = Matrix4::Perspective(m_FOV, GetApp()->GetAspect(), m_nearPlane, m_farPlane);
}