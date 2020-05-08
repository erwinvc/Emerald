#include "stdafx.h"

ShadowRenderer::ShadowRenderer(uint width, uint height) {
	m_width = width;
	m_height = height;
	m_fbo = GetFrameBufferManager()->Create("Shadow", width, height);
	m_fbo->AddBuffer("Depth", TextureParameters(INT_DEPTH24, DATA_DEPTH, NEAREST, REPEAT, T_UNSIGNED_BYTE), FBOAttachment::DEPTH);

	m_fbo->Bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void ShadowRenderer::Begin(HDRPipeline* pipeline)
{
	m_fbo->Bind();
	m_fbo->ClearDepthOnly();
}
void ShadowRenderer::End(HDRPipeline* pipeline) {

}
void ShadowRenderer::OnImGui()
{
	
}