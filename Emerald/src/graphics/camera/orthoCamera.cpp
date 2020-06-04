#include "stdafx.h"

void OrthoCamera::UpdateProjectionMatrix() {
	m_projectionMatrix = glm::ortho(0.0f, m_viewPort.z, 0.0f, m_viewPort.w, -1.0f, 1.0f);
}