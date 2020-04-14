#include "stdafx.h"

GBuffer::GBuffer() : m_fbo(0) {
	m_fbo = GetFrameBufferManager()->Create("GBuffer", FBOScale::FULL, true);
	m_fbo->Bind();

	m_attributesTexture = m_fbo->AddBuffer("Attributes", TextureParameters(RGBA, RGBA, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_colorTexture = m_fbo->AddBuffer("Color", TextureParameters(RGBA, RGBA, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_normalTexture = m_fbo->AddBuffer("Normals", TextureParameters(RGBA16, RGBA, LINEAR, CLAMP_TO_EDGE, T_FLOAT));
}

void GBuffer::BindTextures() {
	m_fbo->GetDepthTexture()->Bind(0);
	m_colorTexture->Bind(1);
	m_normalTexture->Bind(2);
	m_attributesTexture->Bind(3);
}