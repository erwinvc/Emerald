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

	uint32_t TextureDesc::getBytesPerPixel() const {
		switch (format) {
			// 8-bit formats
			case TextureFormat::R:			return 1;
			case TextureFormat::RG:			return 2;
			case TextureFormat::RGB:
			case TextureFormat::SRGB:		return 3;
			case TextureFormat::RGBA8:
			case TextureFormat::SRGBA:		return 4;

			// Half-float (16 bits = 2 bytes per channel)
			case TextureFormat::RGBA16F:	return 8;    // 4 channels @ 16 bits each = 8 bytes
			case TextureFormat::RG16F:		return 4;      // 2 channels @ 16 bits each = 4 bytes
			case TextureFormat::RGB16F:		return 6;     // 3 channels @ 16 bits each = 6 bytes

			// Full-float (32 bits = 4 bytes per channel)
			case TextureFormat::RGBA32F:	return 16;   // 4 channels @ 32 bits each = 16 bytes
			case TextureFormat::RG32F:		return 8;     // 2 channels @ 32 bits each = 8 bytes
			case TextureFormat::RGB32F:		return 12;    // 3 channels @ 32 bits each = 12 bytes

			// Depth / stencil
			case TextureFormat::DEPTH32F:	return 4;        // 32 bits for depth = 4 bytes
			case TextureFormat::DEPTH24STENCIL8: return 4; // Typically 32 bits total = 4 bytes
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getImageFormat() const {
		switch (format) {
			case TextureFormat::R:					return GL_RED;

			case TextureFormat::RG16F:             
			case TextureFormat::RG32F:  
			case TextureFormat::RG:					return GL_RG;

			case TextureFormat::RGB16F:
			case TextureFormat::RGB32F:
			case TextureFormat::RGB:
			case TextureFormat::SRGB:				return GL_RGB;

			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:	
			case TextureFormat::RGBA8:
			case TextureFormat::SRGBA:				return GL_RGBA;

			case TextureFormat::DEPTH24STENCIL8:	return GL_DEPTH_STENCIL;
			case TextureFormat::DEPTH32F:			return GL_DEPTH_COMPONENT;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getInternalFormat() const {
		switch (format) {
			case TextureFormat::R:              return GL_R8;
			case TextureFormat::RG:             return GL_RG8;
			case TextureFormat::RGB:            return GL_RGB8;
			case TextureFormat::RGBA8:          return GL_RGBA8;
			case TextureFormat::RG16F:			return GL_RG16F;
			case TextureFormat::RG32F:			return GL_RG32F;
			case TextureFormat::RGB16F:			return GL_RGB16F;
			case TextureFormat::RGB32F:			return GL_RGB32F;
			case TextureFormat::RGBA16F:        return GL_RGBA16F;
			case TextureFormat::RGBA32F:        return GL_RGBA32F;
			case TextureFormat::SRGB:           return GL_SRGB8;
			case TextureFormat::SRGBA:          return GL_SRGB8_ALPHA8;
			case TextureFormat::DEPTH24STENCIL8:return GL_DEPTH24_STENCIL8;
			case TextureFormat::DEPTH32F:       return GL_DEPTH_COMPONENT32F;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	uint32_t TextureDesc::getDataType() const {
		switch (format) {
			// 8-bit integer
			case TextureFormat::R:
			case TextureFormat::RG:
			case TextureFormat::RGB:
			case TextureFormat::RGBA8:
			case TextureFormat::SRGB:				return GL_UNSIGNED_BYTE;

			// Float-based
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::RG16F:
			case TextureFormat::RG32F:
			case TextureFormat::RGB16F:
			case TextureFormat::RGB32F:				return GL_FLOAT;

				// Depth / stencil
			case TextureFormat::DEPTH24STENCIL8:	return GL_UNSIGNED_INT_24_8;
			case TextureFormat::DEPTH32F:			return GL_FLOAT;

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
			case TextureFormat::RGBA8:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::SRGB:
			case TextureFormat::SRGBA:
			case TextureFormat::RG16F:
			case TextureFormat::RG32F:
			case TextureFormat::RGB16F:
			case TextureFormat::RGB32F:				return GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;

				// Depth-only or depth-stencil
			case TextureFormat::DEPTH32F:			return GL_DEPTH_ATTACHMENT;
			case TextureFormat::DEPTH24STENCIL8:	return GL_DEPTH_STENCIL_ATTACHMENT;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	bool TextureDesc::isColorAttachmentType() const {
		switch (format) {
			case TextureFormat::R:
			case TextureFormat::RG:
			case TextureFormat::RGB:
			case TextureFormat::RGBA8:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::SRGB:
			case TextureFormat::SRGBA:
			case TextureFormat::RG16F:
			case TextureFormat::RG32F:
			case TextureFormat::RGB16F:
			case TextureFormat::RGB32F:
			case TextureFormat::NONE:				return true;

				// Depth-based
			case TextureFormat::DEPTH32F:
			case TextureFormat::DEPTH24STENCIL8:	return false;
		}
		ASSERT(false, "Unhandled texture format");
		return 0;
	}

	bool TextureDesc::isDepthAttachmentType() const {
		return format == TextureFormat::DEPTH32F || format == TextureFormat::DEPTH24STENCIL8;
	}
}