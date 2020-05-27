#include "stdafx.h"

void DirectionalShadow::Draw(HDRPipeline* pipeline, glm::vec3 lightpos) {
	glm::mat4 lightProjection = glm::ortho(-m_size, m_size, -m_size, m_size, m_nearPlane, m_farPlane);
	//glm::mat4 lightProjection = glm::ortho(-40, 40, -40, 40, -40, 40);

	m_lightSpaceMatrix = lightProjection * glm::lookAt(lightpos * glm::vec3(-1, -1, -1), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_fbo->Bind();
	m_fbo->Clear();
	m_shader->Bind();
	m_shader->Set("_LightSpaceMatrix", m_lightSpaceMatrix);

	pipeline->RenderGeometryShadow(ShadowType::DIRECTIONAL);
	m_fbo->Unbind();
}