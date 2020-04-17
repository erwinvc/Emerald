#include "stdafx.h"

ShadowRenderer::ShadowRenderer(uint width, uint height) {
	m_width = width;
	m_height = height;
	m_fbo = GetFrameBufferManager()->Create("Shadow", width, height, true);
	m_fbo->Bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void ShadowRenderer::Begin(RenderingPipeline* pipeline)
{
	m_fbo->Bind();
	m_fbo->ClearDepthOnly();
}
void ShadowRenderer::End(RenderingPipeline* pipeline) {

}
void ShadowRenderer::OnImGui()
{
	
}