#include "eepch.h"
#include "texture.h"
#include "graphics/textureDesc.h"
#include "util/utils.h"
#include "util/fileSystem.h"

#include <stb_image.h>
#include <stb_image_write.h>

namespace emerald {
	Texture::Texture(TextureDesc desc, std::string path) {
		m_desc = desc;
		if (!FileSystem::doesFileExist(path)) {
			//Log::info(path.c_str(), "");
			Log::error("Texture file at {} does not exist!", path);
			return;
		}

		int channelCount;
		int width, height;

		//stbi_set_flip_vertically_on_load(desc.m_flip);
		byte* data = stbi_load(path.c_str(), &width, &height, &channelCount, 4);

		//if (bpc != 3 && bpc != 4) {
		//	LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
		//	stbi_image_free(data);
		//	return false;
		//}

		if (channelCount < 1 || channelCount > 4) Log::error("Unsupported image channel count ({}) {}", channelCount, path.c_str());

		uint32_t size = channelCount * width * height;

		if (data) {
			m_channelCount = channelCount;
			m_width = width;
			m_height = height;
			m_buffer = Buffer<byte>::copy(data, size);
			stbi_image_free(data);
			Log::info("Loaded {}", path.c_str());
		} else Log::error("Failed to load {}", path.c_str());
	}

	Texture::Texture(TextureDesc desc, uint32_t width, uint32_t height, const byte* data, uint32_t dataSize, TextureDataType textureDataType)
		: m_desc(desc), m_width(width), m_height(height) {
		if (textureDataType == TextureDataType::RAW) {
			if (data) {
				m_buffer = Buffer<byte>::copy((byte*)data, getImageMemorySize());
			}
		} else if (textureDataType == TextureDataType::FILE) {
			int channelCount;
			int width, height;

			byte* loadedData = stbi_load_from_memory(data, dataSize, &width, &height, &channelCount, 4);

			if (channelCount < 1 || channelCount > 4) Log::error("Unsupported image channel count ({})", channelCount);

			uint32_t size = channelCount * width * height;

			if (loadedData) {
				m_channelCount = channelCount;
				m_width = width;
				m_height = height;
				m_buffer = Buffer<byte>::copy(loadedData, size);
				stbi_image_free(loadedData);
				Log::info("Loaded texture from memory");
			} else Log::error("Failed to load texture from memory");
		}
	}

	Texture::Texture(TextureDesc desc, uint32_t width, uint32_t height)
		: m_desc(desc), m_width(width), m_height(height) {
	}

	Texture::~Texture() {
		cleanup();
	}

	void Texture::cleanup() const {
		if (m_handle) {
			glDeleteTextures(1, &m_handle);
		}
	}

	void Texture::invalidate() {
		if (m_handle) cleanup();

		glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);

		m_mipmapCount = m_desc.m_hasMipmaps ? utils::calculateMipCount(m_width, m_height) : 1;

		glTextureStorage2D(m_handle, m_mipmapCount, m_desc.getInternalFormat(), m_width, m_height);

		if (m_buffer) {
			auto imageFormat = m_desc.getImageFormat();
			auto dataType = m_desc.getDataType();
			glTextureSubImage2D(m_handle, 0, 0, 0, m_width, m_height, imageFormat, dataType, m_buffer.data());
			if (m_desc.m_hasMipmaps) glGenerateTextureMipmap(m_handle);
		}

		if (!m_desc.m_readWrite) m_buffer.clear();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, m_desc.getWrap());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_desc.getWrap());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_desc.getWrap());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_desc.getFilter(GL_TEXTURE_MIN_FILTER, m_desc.m_hasMipmaps));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_desc.getFilter(GL_TEXTURE_MAG_FILTER, m_desc.m_hasMipmaps));

		if (m_desc.m_hasMipmaps) {
			if (GL_EXT_texture_filter_anisotropic) {
				float value = 0;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
				float amount = glm::min(4.0f, value);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
			} else Log::warn("GL_EXT_texture_filter_anisotropic not supported");
		}
	}

	void Texture::bind(uint32_t slot /*= 0*/) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_handle);
	}

	void Texture::unbind(uint32_t slot /*= 0*/) const {
		glBindTexture(GL_TEXTURE_2D, slot);
	}

	void Texture::resize(uint32_t width, uint32_t height) {
		if (m_width == width && m_height == height) return;
		m_width = width;
		m_height = height;

		invalidate();
	}

	void Texture::saveAsPNG(const std::string& file) {
		if (m_desc.m_readWrite && m_buffer) {
			uint32_t channels = m_desc.getChannelCount();
			stbi_write_png(file.c_str(), getWidth(), getHeight(), channels, m_buffer.data(), channels * getWidth());
			Log::info("Saved texture to {}", file.c_str());
		} else Log::warn("Failed to save texture because texture has no data");
	}
}