#include "stdafx.h"

GBuffer::GBuffer() : m_fbo(0) {
	m_fbo = GetFrameBufferManager()->Create("GBuffer", FBOScale::FULL);
	//m_fbo->AddBuffer("DepthStencil", TextureParameters(INT_DEPTH24STENCIL8, DATA_DEPTHSTENCIL, NEAREST, CLAMP_TO_EDGE, T_UNSIGNED_INT_24_8), FBOAttachment::DEPTHSTENCIL);
	
	m_fbo->AddBuffer("Depth", TextureParameters(INT_DEPTH, DATA_DEPTH, NEAREST, CLAMP_TO_EDGE, T_UNSIGNED_BYTE), FBOAttachment::DEPTH);

	m_fbo->Bind();

	m_attributesTexture = m_fbo->AddBuffer("Attributes", TextureParameters(INT_RGBA, DATA_RGBA, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_colorTexture = m_fbo->AddBuffer("Color", TextureParameters(INT_RGBA, DATA_RGBA, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_normalTexture = m_fbo->AddBuffer("Normals", TextureParameters(INT_RGBA16, DATA_RGBA, LINEAR, CLAMP_TO_EDGE, T_FLOAT));
}

void GBuffer::BindTextures() {
	m_fbo->GetDepthTexture()->Bind(0);
	m_colorTexture->Bind(1);
	m_normalTexture->Bind(2);
	m_attributesTexture->Bind(3);
}