#pragma once

class TextureArray {
	TextureParameters m_params;
	uint m_handle;
	uint m_index;
	uint m_size;
	uint m_layers;

public:
	TextureArray(uint size, uint layers, TextureParameters params) : m_handle(0), m_index(0), m_size(size), m_layers(layers), m_params(params) {
		GL(glGenTextures(1, &m_handle));
		GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));

		GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_params.GetFilter(GL_TEXTURE_MIN_FILTER)));
		GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_params.GetFilter(GL_TEXTURE_MAG_FILTER)));
		GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_params.GetWrap()));
		GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_params.GetWrap()));
		//GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4));

		GL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 5, params.GetInternalFormatSized(), m_size, m_size, m_layers));

		//if (GLEW_EXT_texture_filter_anisotropic) {
		//	float value = 0;
		//	GL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value));
		//	float amount = Math::Min(8.0f, value);
		//	GL(glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16));
		//} else LOG_WARN("GL_EXT_texture_filter_anisotropic not supported");
	}

	int AddTexture(byte* data) {
		GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
		GL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_index, m_size, m_size, 1, m_params.GetInternalFormat(), m_params.GetType(), data));
		return m_index++;
	}

	void GenerateMipmaps() {
		GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
		GL(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
	}

	void Bind(uint slot = 0) const {
		GL(glActiveTexture(GL_TEXTURE0 + slot));
		GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
	}

	void Unbind() const {
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
};