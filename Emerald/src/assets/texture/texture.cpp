#include "stdafx.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

Texture::Texture(int32 width, int32 height, byte* data, TextureParameters params, bool keepData) : m_params(params), m_textureID(0), m_width(width), m_height(height), m_path(""), m_keepData(keepData) {
	SetData(data);
}

Texture::Texture(int32 width, int32 height, TextureParameters params) : m_params(params), m_textureID(0), m_width(width), m_height(height), m_path(""), m_keepData(false) {
	SetData(NULL);
}

Texture::Texture(const String& path, TextureParameters params, bool keepData) : m_params(params), m_textureID(0), m_width(0), m_height(0), m_path(path), m_keepData(keepData) {
	if (!FileSystem::DoesFileExist(path)) {
		LOG_ERROR("[~gTexture~x] file at ~1%s~x does not exist!", path.c_str());
		return;
	}

	int bpc;

	stbi_set_flip_vertically_on_load(params.GetFlipY() ? 1 : 0);
	byte* data = stbi_load(m_path.c_str(), (int*)&m_width, (int*)&m_height, &bpc, 4);

	if (bpc != 3 && bpc != 4) {
		LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
		return;
	}

	if (data) {
		SetData(data);
		stbi_image_free(data);
		LOG("[~gTexture~x] Loaded ~1%s", path.c_str());
	} else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", path.c_str());
}

void Texture::SetData(byte* data) {
	if (m_textureID == 0) GL(glGenTextures(1, &m_textureID));

	GL(glBindTexture(GL_TEXTURE_2D, m_textureID));

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_params.GetFilter(GL_TEXTURE_MIN_FILTER)));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_params.GetFilter(GL_TEXTURE_MAG_FILTER)));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_params.GetWrap()));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_params.GetWrap()));

	GL(glTexImage2D(GL_TEXTURE_2D, 0, m_params.GetFormat(), m_width, m_height, 0, GL_RGBA, m_params.GetType(), data));
	GL(glGenerateMipmap(GL_TEXTURE_2D));

	if (m_keepData) {
		m_data = new byte[2 * 4 * m_width * m_height];
		memcpy(m_data, data, 2 * 4 * m_width * m_height);
	}
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

	//float val;
	//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Math::_min(16.0f, val));
}

Texture::~Texture() {
	glDeleteTextures(1, &m_textureID);
	if (m_keepData) {
		delete[] m_data;
	}
}

void Texture::Bind(uint slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(uint slot) {
	glBindTexture(GL_TEXTURE_2D, slot);
}

#undef STB_IMAGE_IMPLEMENTATION