#pragma once

namespace emerald {
	enum TextureFormat : int {
		NONE,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,

		SRGB,
		SRGBA,

		DEPTH32F,
		DEPTH24STENCIL8,

		Depth = DEPTH24STENCIL8
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

	class TextureDesc final {
	public:
		TextureFormat m_format = RGBA;
		TextureFilter m_filter = LINEAR;
		TextureWrap m_wrap = REPEAT;
		uint32_t m_anisotropyLevel = 0;
		bool m_hasMipmaps = false;
		bool m_readWrite = false;

		std::string wrapToString(TextureWrap wrap) const;
		std::string filterToString(TextureFilter filter) const;

		uint32_t getWrap() const { return m_wrap; }
		uint32_t getFilter(int type, bool mipmap) const;

		uint32_t getChannelCount() const;
		uint32_t getImageFormat() const;
		uint32_t getInternalFormat() const;
		uint32_t getDataType() const;
	};
}