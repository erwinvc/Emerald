#include "stdafx.h"

void MaterialCallbackTexture::Callback() {
	m_texture->Bind(m_slot);
}

void MaterialCallbackGBuffer::Callback() {
	ShaderUniformBuffer::SetGL(m_uniformLocation, 0);
	ShaderUniformBuffer::SetGL(m_uniformLocation1, 1);
	ShaderUniformBuffer::SetGL(m_uniformLocation2, 2);
	ShaderUniformBuffer::SetGL(m_uniformLocation3, 3);

	GetClient()->pipeline->GetGBuffer()->BindTextures();
}

