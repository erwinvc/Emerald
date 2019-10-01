#include "stdafx.h"

GBuffer::GBuffer(uint width, uint height) : m_fbo(0), m_width(width), m_height(height) {
	m_fbo = GetFrameBufferManager()->Create("GBuffer", FBOScale::FULL);
	m_fbo->Bind();

	m_attributesTexture = m_fbo->AddColorBuffer("Attributes", TextureParameters(RGB, RGB, NEAREST, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_colorTexture = m_fbo->AddColorBuffer("Color", TextureParameters(RGB, RGB, NEAREST, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_normalTexture = m_fbo->AddColorBuffer("Normals", TextureParameters(RGB16, RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	m_positionTexture = m_fbo->AddColorBuffer("Positions", TextureParameters(RGB16, RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
}

void GBuffer::BindTextures() {
	m_attributesTexture->Bind(0);
	m_colorTexture->Bind(1);
	m_normalTexture->Bind(2);
	m_positionTexture->Bind(3);
}