#pragma once

namespace emerald {
	enum class TextureFormat : int {
		NONE,
		R,
		RG,
		RGB,
		RGBA8,

		RG16F,
		RG32F,
		RGB16F,
		RGB32F,
		RGBA16F,
		RGBA32F,

		SRGB,
		SRGBA,

		DEPTH32F,
		DEPTH24STENCIL8,
	};

	enum class TextureFilter {
		LINEAR,
		NEAREST
	};

	enum class TextureWrap {
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
		std::string name;
		TextureFormat format = TextureFormat::RGBA8;
		TextureFilter filter = TextureFilter::LINEAR;
		TextureWrap wrap = TextureWrap::REPEAT;
		MSAA samples = MSAA::NONE;
		uint32_t anisotropyLevel = 0;
		bool hasMipmaps = false;
		bool readWrite = false;
		bool isImmutable = true;

		std::string wrapToString(TextureWrap wrap) const;
		std::string filterToString(TextureFilter filter) const;

		uint32_t getWrap() const { return (uint32_t)wrap; }
		uint32_t getFilter(int type, bool mipmap) const;

		uint32_t getSamples() const;
		uint32_t getTarget() const;
		uint32_t getBytesPerPixel() const;
		uint32_t getImageFormat() const;
		uint32_t getInternalFormat() const;
		uint32_t getDataType() const;
		uint32_t textureFormatToAttachmentType(uint32_t colorAttachmentIndex = 0) const;
		bool isColorAttachmentType() const;
		bool isDepthAttachmentType() const;
	};
}