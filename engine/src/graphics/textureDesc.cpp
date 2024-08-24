#include "eepch.h"
#include "textureDesc.h"

namespace emerald {
	std::string TextureDesc::wrapToString(TextureWrap wrap) const {
		switch (wrap) {
			case CLAMP: return "CLAMP";
			case REPEAT: return "REPEAT";
			case MIRRORED_REPEAT: return "MIRRORED_REPEAT";
			case CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
			case CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
		}
		return "NULL";
	}

	std::string TextureDesc::filterToString(TextureFilter filter) const {
		switch (filter) {
			case LINEAR: return "LINEAR";
			case NEAREST: return "NEAREST";
		}
		return "NULL";
	}

	GLenum TextureDesc::getFilter(int type, bool mipmap) const {
		switch (type) {
			case GL_TEXTURE_MIN_FILTER:
			{
				switch (filter) {
					case LINEAR: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
					case NEAREST: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST;
				}
			}break;
			case GL_TEXTURE_MAG_FILTER: return filter == LINEAR ? GL_LINEAR : GL_NEAREST; break;
		}
		return GL_LINEAR;
	}

	uint32_t TextureDesc::getChannelCount() const {
		switch (format) {
			case TextureFormat::RGB:
			case TextureFormat::SRGB:    return 3;
			case TextureFormat::SRGBA:
			case TextureFormat::RGBA:    return 4;
			case TextureFormat::RGBA16F: return 2 * 4;
			case TextureFormat::RGBA32F: return 4 * 4;
		}
		return 0;
	}

	uint32_t TextureDesc::getImageFormat() const {
		switch (format) {
			case TextureFormat::RGB:     return GL_RGB;
			case TextureFormat::SRGB:    return GL_RGB;
			case TextureFormat::SRGBA:    return GL_RGBA;
			case TextureFormat::RGBA:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F: return GL_RGBA;
		}
		return 0;
	}

	uint32_t TextureDesc::getInternalFormat() const {
		switch (format) {
			case TextureFormat::RGB:             return GL_RGB8;
			case TextureFormat::SRGB:            return GL_SRGB8;
			case TextureFormat::SRGBA:           return GL_SRGB8_ALPHA8;
			case TextureFormat::RGBA:            return GL_RGBA8;
			case TextureFormat::RGBA16F:         return GL_RGBA16F;
			case TextureFormat::RGBA32F:         return GL_RGBA32F;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			case TextureFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
		}
		return 0;
	}

	uint32_t TextureDesc::getDataType() const {
		switch (format) {
			case TextureFormat::RGB:
			case TextureFormat::SRGB:
			case TextureFormat::RGBA:    return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F: return GL_FLOAT;
		}
		return 0;
	}
}