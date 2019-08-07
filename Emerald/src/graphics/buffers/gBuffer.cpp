#include "stdafx.h"

GBuffer::GBuffer(uint width, uint height) : m_fbo(0), m_width(width), m_height(height) {
	m_fbo = GetFrameBufferManager()->Create("GBuffer", m_width, m_height);
	m_fbo->Bind();

	m_miscTexture = m_fbo->AddColorBuffer(TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE));
	m_colorTexture = m_fbo->AddColorBuffer(TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE));
	m_normalTexture = m_fbo->AddColorBuffer(TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT));
	m_positionTexture = m_fbo->AddColorBuffer(TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT));
}

void GBuffer::BindTextures() {
	m_miscTexture->Bind(0);
	m_colorTexture->Bind(1);
	m_normalTexture->Bind(2);
	m_positionTexture->Bind(3);
}