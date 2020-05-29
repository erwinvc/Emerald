#include "stdafx.h"

SSRRenderer::SSRRenderer() {
	m_shader = GetShaderManager()->Get("SSR");
	m_shader->Bind();
	m_shader->Set("_Depth", 0);
	m_shader->Set("_GAlbedo", 1);
	m_shader->Set("_GNormal", 2);
	m_shader->Set("_GMisc", 3);
	m_shader->Set("_HDR", 4);

	m_simpleShader = GetShaderManager()->Get("Simple");
	m_simpleShader->Bind();
	m_simpleShader->Set("_SSR", 0);

	m_fbo = GetFrameBufferManager()->Create("SSR", FBOScale::FULL);
	m_texture = m_fbo->AddBuffer("SSR", TextureParameters(INT_RGB16, DATA_RGB, LINEAR, CLAMP_TO_EDGE, T_FLOAT));

	m_quad = MeshGenerator::Quad();
	LOG("[~bRenderer~x] SSR renderer initialized");
}

void SSRRenderer::Draw(HDRPipeline* pipeline) {
	auto& pSSR = GetProfiler()->StartGL(ProfilerDataType::SSR);

	if (m_enabled) {
		m_fbo->Bind();
		m_fbo->Clear();

		m_shader->Bind();
		pipeline->GetGBuffer()->BindTextures();
		pipeline->GetHDRTexture()->Bind(4);
		m_quad->Bind();
		m_quad->Draw();

		m_fbo->Unbind();

		m_simpleShader->Bind();
		pipeline->m_hdrFBO->Bind();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_EQUAL);
		m_texture->Bind(0);

		m_quad->Bind();
		m_quad->Draw();

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
	}
	pSSR.End();
}

void SSRRenderer::OnImGui() {

}