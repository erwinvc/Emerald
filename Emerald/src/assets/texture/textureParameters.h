#pragma once

enum TextureWrap {
	CLAMP = 0x2900,
	REPEAT = 0x2901,
	MIRRORED_REPEAT = 0x8370,
	CLAMP_TO_EDGE = 0x812F,
	CLAMP_TO_BORDER = 0x812D
};

enum TextureFilter {
	LINEAR,
	NEAREST
};

enum TextureInternalFormat {
	INT_RED = GL_RED,
	INT_RED32 = GL_R32F,
	INT_RG = GL_RG,
	INT_RG32 = GL_RG32F,
	INT_RGBA = GL_RGBA,
	INT_SRGB = GL_SRGB,
	INT_SRGBA = GL_SRGB_ALPHA,
	INT_RGB = GL_RGB,
	INT_RGBA32 = GL_RGBA32F,
	INT_RGB32 = GL_RGB32F,
	INT_RGBA16 = GL_RGBA16F,
	INT_RGB16 = GL_RGB16F,
	INT_DEPTH = GL_DEPTH_COMPONENT,
	INT_DEPTH32 = GL_DEPTH_COMPONENT32,
	INT_DEPTH24 = GL_DEPTH_COMPONENT24,
	INT_DEPTHSTENCIL = GL_DEPTH_STENCIL,
	INT_DEPTH24STENCIL8 = GL_DEPTH24_STENCIL8,
	INT_DEPTH32STENCIL8 = GL_DEPTH32F_STENCIL8,
};

enum TextureDataFormat {
	DATA_UNK = 0,
	DATA_RED = GL_RED,
	DATA_RG = GL_RG,
	DATA_RGB = GL_RGB,
	DATA_RGBA = GL_RGBA,
	DATA_STENCIL = GL_STENCIL_INDEX,
	DATA_DEPTH = GL_DEPTH_COMPONENT,
	DATA_DEPTHSTENCIL = GL_DEPTH_STENCIL
};

enum TextureType {
	T_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	T_UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8_EXT,
	T_FLOAT = GL_FLOAT,
};

class TextureParameters final {
private:
	TextureInternalFormat m_internalFormat;
	TextureDataFormat m_dataFormat;
	TextureFilter m_filter;
	TextureWrap m_wrap;
	TextureType m_type;
	bool m_flipY;

	String WrapToString(TextureWrap wrap) const;
	String InternalFormatToString(TextureInternalFormat format) const;
	String DataFormatToString(TextureDataFormat format) const;
	String FilterToString(TextureFilter filter) const;
	String TypeToString(TextureType type) const;
	int BaseInternalFormatToSizedInternalFormat(TextureInternalFormat format) const;

public:

	TextureParameters(TextureInternalFormat internalFormat = INT_RGBA, TextureDataFormat dataFormat = DATA_RGBA, TextureFilter filter = LINEAR, TextureWrap wrap = REPEAT, TextureType type = T_UNSIGNED_BYTE, bool flipY = true)
		: m_internalFormat(internalFormat), m_dataFormat(dataFormat), m_filter(filter), m_wrap(wrap), m_type(type), m_flipY(flipY) {}

	inline bool GetFlipY() const { return m_flipY; }
	inline int GetInternalFormat() const { return m_internalFormat; }
	inline int GetDataFormat() const { return m_dataFormat; }
	inline int GetWrap() const { return m_wrap; }
	inline int GetType() const { return m_type; }

	int GetInternalFormatSized();
	int GetFilter(int type, bool mipmap) const;
	void SetDataFormat(TextureDataFormat dataFormat);
	void SetDataFormatFromChannelCount(int count);
	int GetChannelCount();

	String GetAsString() const;
};