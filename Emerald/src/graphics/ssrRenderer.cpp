#include "stdafx.h"

SSRRenderer::SSRRenderer() {
	m_shader = GetShaderManager()->Get("SSR");
	m_shader->Bind();
	m_shader->Set("_Depth", 0);
	m_shader->Set("_GAlbedo", 1);
	m_shader->Set("_GNormal", 2);
	m_shader->Set("_GMisc", 3);
	m_shader->Set("_HDR", 4);

	m_fbo = GetFrameBufferManager()->Create("SSR", FBOScale::FULL, false);
	m_texture = m_fbo->AddBuffer("SSR", TextureParameters(RGB, RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	m_quad = MeshGenerator::Quad();
}

void SSRRenderer::Draw(RenderingPipeline* pipeline) {
	m_fbo->Bind();
	m_fbo->Clear();
	m_shader->Bind();
	pipeline->GetGBuffer()->BindTextures();
	pipeline->GetHDRTexture()->Bind(4);
	m_shader->Set("_Projection", GetCamera()->GetProjectionMatrix());
	m_shader->Set("_View", GetCamera()->GetViewMatrix());
	m_shader->Set("_InverseProjection", glm::inverse(GetCamera()->GetProjectionMatrix()));
	m_shader->Set("_InverseView", glm::inverse(GetCamera()->GetViewMatrix()));
	m_shader->Set("_CameraPosition", GetCamera()->transform.m_position);

	m_quad->Bind();
	m_quad->Draw();

	m_fbo->Unbind();
}