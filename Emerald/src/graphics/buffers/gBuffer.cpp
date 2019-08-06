#include "stdafx.h"

GBuffer::GBuffer(AssetRef<FrameBuffer> fbo, uint width, uint height) : m_fbo(fbo), m_width(width), m_height(height) {
    m_fbo->Bind();

    m_miscTexture = NEW(Texture(m_width, m_height, TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE)));
    m_colorTexture = NEW(Texture(m_width, m_height, TextureParameters(RGBA, LINEAR, REPEAT, T_UNSIGNED_BYTE)));
    m_normalTexture = NEW(Texture(m_width, m_height, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT)));
    m_positionTexture = NEW(Texture(m_width, m_height, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT)));

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