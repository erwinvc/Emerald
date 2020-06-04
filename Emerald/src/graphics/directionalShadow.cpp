#include "stdafx.h"

DirectionalShadow::DirectionalShadow() {
	m_shaderChunk = GetShaderManager()->Get("DirectionalShadowChunk");
	m_shader = GetShaderManager()->Get("DirectionalShadow");
	m_fbo = GetFrameBufferManager()->Create("DirectionalShadow", SIZE, SIZE, Color(1.0f, 1.0f, 0.0f, 0.0f));
	m_fbo->AddBuffer("Depth", TextureParameters(INT_DEPTH, DATA_DEPTH, LINEAR, REPEAT, T_UNSIGNED_BYTE), FBOAttachment::DEPTH);
	m_texture = m_fbo->AddBuffer("DirectionalShadow", TextureParameters(INT_RG32, DATA_RGBA, LINEAR, REPEAT, T_FLOAT), FBOAttachment::COLOR);
}

void DirectionalShadow::Draw(HDRPipeline* pipeline, glm::vec3 lightpos) {
	glm::mat4 lightProjection = glm::ortho(-m_size, m_size, -m_size, m_size, m_nearPlane, m_farPlane);
	m_lightSpaceMatrix = lightProjection * glm::lookAt(lightpos * glm::vec3(-1, -1, -1), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_fbo->Bind();
	m_fbo->Clear();

	m_shaderChunk->Bind();
	m_shaderChunk->Set("_LightSpaceMatrix", m_lightSpaceMatrix);
	
	m_shader->Bind();
	m_shader->Set("_LightSpaceMatrix", m_lightSpaceMatrix);
	
	pipeline->RenderGeometryShadow(ShadowType::DIRECTIONAL);
	m_fbo->Unbind();
}

void DirectionalShadow::OnImGui() {
	UI::Begin();
	UI::Float("Far", &m_farPlane, 2.0f, 512.0f);
	UI::Float("Near", &m_nearPlane, -200.0f, 1.0f);
	UI::Float("Size", &m_size, 1.0f, 64.0f);
	UI::End();
}