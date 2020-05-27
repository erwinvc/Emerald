#include "stdafx.h"

void DepthCubemap::Draw(HDRPipeline* pipeline, glm::vec3 lightpos) {
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	shadowProj = glm::perspective(glm::radians(90.0f), aspect, m_nearPlane, m_farPlane);

	shadowTransforms.clear();
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	m_fbo->Bind();
	m_fbo->ClearDepthOnly();
	m_shader->Bind();
	m_shader->Set("shadowMatrices", shadowTransforms.data(), shadowTransforms.size());
	m_shader->Set("far_plane", m_farPlane);
	m_shader->Set("lightPos", lightpos);

	pipeline->RenderGeometryShadow(ShadowType::OMNIDIRECTIONAL);
	m_fbo->Unbind();
}