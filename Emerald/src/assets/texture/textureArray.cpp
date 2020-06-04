#include "stdafx.h"



TextureArray::TextureArray(uint size, uint layers, TextureParameters params) : m_handle(0), m_index(0), m_size(size), m_layers(layers), m_params(params) {
	GL(glGenTextures(1, &m_handle));
	GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));

	GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_params.GetFilter(GL_TEXTURE_MIN_FILTER, true)));
	GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_params.GetFilter(GL_TEXTURE_MAG_FILTER, true)));
	GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_params.GetWrap()));
	GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_params.GetWrap()));

	GL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 6, params.GetInternalFormatSized(), m_size, m_size, m_layers));

	if (GLEW_EXT_texture_filter_anisotropic) {
		float value = 0;
		GL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value));
		float amount = Math::Min(8.0f, value);
		GL(glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount));
	} else LOG_WARN("GL_EXT_texture_filter_anisotropic not supported");
}

int TextureArray::AddTexture(const LoadedTextureArrayTexture& textureData) {
	if (textureData.size.x != m_size || textureData.size.y != m_size) LOG_ERROR("[~gTexture~x] ~1%s~x texture size (%u %u) does not match texture array size (%u %u)", textureData.path.GetFileName().c_str(), textureData.size.x, textureData.size.y, m_size, m_size);
	GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
	GL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_index, m_size, m_size, 1, textureData.params.GetDataFormat(), m_params.GetType(), textureData.data));
	return m_index++;
}

void TextureArray::GenerateMipmaps() {
	GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
	GL(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
}

void TextureArray::Bind(uint slot) const {
	GL(glActiveTexture(GL_TEXTURE0 + slot));
	GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
}

void TextureArray::Unbind() const {
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}