#pragma once

namespace emerald {
	enum TextureFormat : int {
		NONE,
		RGB,
		RGBA8F,
		RGBA16F,
		RGBA32F,

		SRGB,
		SRGBA,

		DEPTH32F,
		DEPTH24STENCIL8,
	};

	enum TextureFilter {
		LINEAR,
		NEAREST
	};

	enum TextureWrap {
		CLAMP = 0x2900,
		REPEAT = 0x2901,
		MIRRORED_REPEAT = 0x8370,
		CLAMP_TO_EDGE = 0x812F,
		CLAMP_TO_BORDER = 0x812D
	};

	enum class TextureShape {
		TEXTURE_2D,
		TEXTURE_CUBE,
		TEXTURE_2D_ARRAY,
		TEXTURE_3D
	};

	enum class MSAA {
		NONE = 0,
		X2 = 2,
		X4 = 4,
		X8 = 8,
		X16 = 16
	};

	class TextureDesc final {
	public:
		TextureFormat format = RGBA8F;
		TextureFilter filter = LINEAR;
		TextureWrap wrap = REPEAT;
		MSAA samples = MSAA::NONE;
		uint32_t anisotropyLevel = 0;
		bool hasMipmaps = false;
		bool readWrite = false;
		bool isImmutable = true;

		std::string wrapToString(TextureWrap wrap) const;
		std::string filterToString(TextureFilter filter) const;

		uint32_t getWrap() const { return wrap; }
		uint32_t getFilter(int type, bool mipmap) const;

		uint32_t getSamples() const;
		uint32_t getTarget() const;
		uint32_t getChannelCount() const;
		uint32_t getImageFormat() const;
		uint32_t getInternalFormat() const;
		uint32_t getDataType() const;
		uint32_t textureFormatToAttachmentType(uint32_t colorAttachmentIndex = 0) const;
		bool isColorAttachmentType()const;
	};
}