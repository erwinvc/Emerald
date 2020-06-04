#include "stdafx.h"


void MaterialCallbackTexturePtr::Callback() {
	if (m_texture) {
		(*m_texture)->Bind(m_slot);
	}
}

void MaterialCallbackTexture::Callback() {
	m_texture->Bind(m_slot);
}

void MaterialCallbackTextureArray::Callback() {
	m_texture->Bind(m_slot);
}

void MaterialCallbackGBuffer::SetUniformLocation(Shader* shader) {
	m_uniformLocation = shader->GetUniformLocation("_Depth");
	m_uniformLocation1 = shader->GetUniformLocation("_GAlbedo");
	m_uniformLocation2 = shader->GetUniformLocation("_GNormal");
	m_uniformLocation3 = shader->GetUniformLocation("_GMisc");
}

void MaterialCallbackGBuffer::Callback() {
	ShaderUniformBuffer::SetGL(m_uniformLocation, 0);
	ShaderUniformBuffer::SetGL(m_uniformLocation1, 1);
	ShaderUniformBuffer::SetGL(m_uniformLocation2, 2);
	ShaderUniformBuffer::SetGL(m_uniformLocation3, 3);

	GetClient()->pipeline->GetGBuffer()->BindTextures();
}

