#include "stdafx.h"

void GBuffer::Initialize() {
    m_fbo = new FrameBuffer("GBuffer", m_width, m_height);
    m_fbo->Bind();

    m_miscTexture = new Texture(m_width, m_height, TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE));
    m_colorTexture = new Texture(m_width, m_height, TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE));
    m_normalTexture = new Texture(m_width, m_height, TextureParameters(RGBA16, LINEAR, REPEAT, T_FLOAT));
    m_positionTexture = new Texture(m_width, m_height, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT));

	m_fbo->AddColorBuffer(m_miscTexture);
	m_fbo->AddColorBuffer(m_colorTexture);
	m_fbo->AddColorBuffer(m_normalTexture);
	m_fbo->AddColorBuffer(m_positionTexture);
}

void GBuffer::BindTextures() {
    m_miscTexture->Bind(0);
    m_colorTexture->Bind(1);
    m_normalTexture->Bind(2);
    m_positionTexture->Bind(3);
}