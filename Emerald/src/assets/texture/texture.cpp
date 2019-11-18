#include "stdafx.h"

Texture::Texture(int32 width, int32 height, byte* data, bool hasMipmaps, TextureParameters params, bool keepData) : m_params(params), m_textureID(0), m_width(width), m_height(height), m_path(""), m_data(nullptr), m_hasMipmaps(hasMipmaps), m_keepData(keepData) {
	SetData(data);
}

Texture::Texture(int32 width, int32 height, bool hasMipmaps, TextureParameters params, bool keepData) : m_params(params), m_textureID(0), m_width(width), m_height(height), m_path(""), m_data(nullptr), m_hasMipmaps(hasMipmaps), m_keepData(keepData) {
	SetData(nullptr);
}

void Texture::SetData(byte* data) {
	GL(glGenTextures(1, &m_textureID));

	GL(glBindTexture(GL_TEXTURE_2D, m_textureID));

	if (m_hasMipmaps) {
		GL(glTexStorage2D(GL_TEXTURE_2D, 7, m_params.GetInternalFormatSized(), m_width, m_height));
		GL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_params.GetFormat(), m_params.GetType(), data));
		GL(glGenerateMipmap(GL_TEXTURE_2D));
	} else {
		GL(glTexImage2D(GL_TEXTURE_2D, 0, m_params.GetInternalFormat(), m_width, m_height, 0, m_params.GetFormat(), m_params.GetType(), data));
	}

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_params.GetFilter(GL_TEXTURE_MIN_FILTER, m_hasMipmaps)));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_params.GetFilter(GL_TEXTURE_MAG_FILTER, m_hasMipmaps)));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_params.GetWrap()));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_params.GetWrap()));

	if (m_hasMipmaps) {
		if (GLEW_EXT_texture_filter_anisotropic) {
			float value = 0;
			GL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value));
			float amount = Math::Min(8.0f, value);
			GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount));
		} else LOG_WARN("GL_EXT_texture_filter_anisotropic not supported");
	}

	int size = m_width * m_height * m_params.GetChannelCount();

	if (m_keepData && data != nullptr) {
		m_data = new byte[size];
		memcpy(m_data, data, size);
	}
}

Texture::~Texture() {
	Cleanup();
}

void Texture::Bind(uint slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(uint slot) {
	glBindTexture(GL_TEXTURE_2D, slot);
}

void Texture::Cleanup() {
	glDeleteTextures(1, &m_textureID);
	if (m_keepData) {
		delete[] m_data;
	}
}

void Texture::Resize(int width, int height) {
	if (m_width == width && m_height == height) return;
	m_width = width;
	m_height = height;

	Bind();
	GL(glTexImage2D(GL_TEXTURE_2D, 0, m_params.GetInternalFormat(), m_width, m_height, 0, m_params.GetFormat(), m_params.GetType(), nullptr));
	GL(glGenerateMipmap(GL_TEXTURE_2D));
}