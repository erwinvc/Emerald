#include "eepch.h"
#include "textureDesc.h"

namespace emerald {
	std::string TextureDesc::wrapToString(TextureWrap wrap) const {
		switch (wrap) {
			case TextureWrap::CLAMP: return "CLAMP";
			case TextureWrap::REPEAT: return "REPEAT";
			case TextureWrap::MIRRORED_REPEAT: return "MIRRORED_REPEAT";
			case TextureWrap::CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
			case TextureWrap::CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
		}
		ASSERT(false, "Unhandled wrap format");
		return "NULL";
	}

	std::string TextureDesc::filterToString(TextureFilter filter) const {
		switch (filter) {
			case TextureFilter::LINEAR: return "LINEAR";
			case TextureFilter::NEAREST: return "NEAREST";
		}
		ASSERT(false, "Unhandled filter format");
		return "NULL";
	}

	GLenum TextureDesc::getFilter(int type, bool mipmap) const {
		switch (type) {
			case GL_TEXTURE_MIN_FILTER:
			{
				switch (filter) {
					case TextureFilter::LINEAR: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
					case TextureFilter::NEAREST: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST;
				}
			}break;
			case GL_TEXTURE_MAG_FILTER: return filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST; break;
		}
		return GL_LINEAR;
	}
	uint32_t TextureDesc::getSamples() const {
		return (uint32_t)samples;
	}

	uint32_t TextureDesc::getTarget() const {
		return samples != MSAA::NONE ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	uint32_t TextureDesc::getChannelCount() const {
		switch (format) {
			case TextureFormat::R:      return 1;
			case TextureFormat::RG:     return 2;
			case TextureFormat::RGB:
			case TextureFormat::SRGB:    return 3;
			case TextureFormat::SRGBA:
			case TextureFormat::RGBA8F:    return 4;
			case TextureFormat::RGBA16F: return 2 * 4;
			case TextureFormat::RGBA32F: return 4 * 4;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getImageFormat() const {
		switch (format) {
			case TextureFormat::R:					return GL_RED;
			case TextureFormat::RG:					return GL_RG;
			case TextureFormat::RGB:				return GL_RGB;
			case TextureFormat::SRGB:				return GL_RGB;
			case TextureFormat::SRGBA:				return GL_RGBA;
			case TextureFormat::RGBA8F:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:			return GL_RGBA;
			case TextureFormat::DEPTH24STENCIL8:	return GL_DEPTH_STENCIL;
			case TextureFormat::DEPTH32F:			return GL_DEPTH_COMPONENT;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getInternalFormat() const {
		switch (format) {
			case TextureFormat::R: 				 return GL_R8;
			case TextureFormat::RG:				 return GL_RG8;
			case TextureFormat::RGB:             return GL_RGB8;
			case TextureFormat::SRGB:            return GL_SRGB8;
			case TextureFormat::SRGBA:           return GL_SRGB8_ALPHA8;
			case TextureFormat::RGBA8F:          return GL_RGBA8;
			case TextureFormat::RGBA16F:         return GL_RGBA16F;
			case TextureFormat::RGBA32F:         return GL_RGBA32F;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			case TextureFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getDataType() const {
		switch (format) {
			case TextureFormat::R:
			case TextureFormat::RG:
			case TextureFormat::RGB:
			case TextureFormat::SRGB:
			case TextureFormat::RGBA8F:					return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:				return GL_FLOAT;
			case TextureFormat::DEPTH24STENCIL8:		return GL_UNSIGNED_INT_24_8;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::textureFormatToAttachmentType(uint32_t colorAttachmentIndex) const {
		switch (format) {
			case TextureFormat::NONE:
			case TextureFormat::R:
			case TextureFormat::RG:
			case TextureFormat::RGB:
			case TextureFormat::RGBA8F:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::SRGB:
			case TextureFormat::SRGBA:			return GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
			case TextureFormat::DEPTH32F:			return GL_DEPTH_ATTACHMENT;
			case TextureFormat::DEPTH24STENCIL8:	return GL_DEPTH_STENCIL_ATTACHMENT;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	bool TextureDesc::isColorAttachmentType() const {
		switch (format) {
			case TextureFormat::NONE:
			case TextureFormat::R:
			case TextureFormat::RG:
			case TextureFormat::RGB:
			case TextureFormat::RGBA8F:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::SRGB:
			case TextureFormat::SRGBA:			return true;
			case TextureFormat::DEPTH32F:
			case TextureFormat::DEPTH24STENCIL8:	return false;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}
}