#pragma once

enum TextureWrap {
	CLAMP = 0x2900,
	REPEAT = 0x2901,
	MIRRORED_REPEAT = 0x8370,
	CLAMP_TO_EDGE = 0x812F,
	CLAMP_TO_BORDER = 0x812D
};

enum TextureFilter {
	//LINEARMIPMAP,
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

	String WrapToString(TextureWrap wrap) const {
		switch (wrap) {
			case CLAMP: return "CLAMP";
			case REPEAT: return "REPEAT";
			case MIRRORED_REPEAT: return "MIRRORED_REPEAT";
			case CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
			case CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
		}
		return "NULL";
	}

	String InternalFormatToString(TextureInternalFormat format) const {
		switch (format) {
			case INT_RED: return "RED";
			case INT_RG: return "RG";
			case INT_RGBA: return "RGBA";
			case INT_RGB: return "RGB";
			case INT_RGBA32: return "RGBA32";
			case INT_RGB32: return "RGB32";
			case INT_RGBA16: return "RGBA16";
			case INT_RGB16: return "RGB16";
			case INT_DEPTH: return "DEPTH";
			case INT_SRGBA: return "SRGBA";
			case INT_SRGB: return "SRGB";
			case INT_DEPTH32: return "DEPTH32";
			case INT_DEPTH24: return "DEPTH24";
			case INT_DEPTHSTENCIL: return "DEPTHSTENCIL";
			case INT_DEPTH24STENCIL8: return "DEPTH24STENCIL8";
			case INT_DEPTH32STENCIL8: return "IDEPTH32STENCIL8";
		}
		return "NULL";
	}

	String DataFormatToString(TextureDataFormat format) const {
		switch (format) {
			case DATA_RED: return "RED";
			case DATA_RG: return "RG";
			case DATA_RGB: return "RGB";
			case DATA_RGBA: return "RGBA";
			case DATA_STENCIL: return "STENCIL";
			case DATA_DEPTH: return "DEPTH";
			case DATA_DEPTHSTENCIL: return "DEPTHSTENCIL";
		}
		return "NULL";
	}

	String FilterToString(TextureFilter filter) const {
		switch (filter) {
			//case LINEARMIPMAP: return "LINEARMIPMAP";
			case LINEAR: return "LINEAR";
			case NEAREST: return "NEAREST";
		}
		return "NULL";
	}

	String TypeToString(TextureType type) const {
		switch (type) {
			case T_UNSIGNED_BYTE: return "UNSIGNED_BYTE";
			case T_FLOAT: return "FLOAT";
			case T_UNSIGNED_INT_24_8: return "UNSIGNED_INT_24_8";
		}
		return "NULL";
	}

	int BaseInternalFormatToSizedInternalFormat(TextureInternalFormat format) const {
		switch (format) {
			case INT_RED: return GL_R8;
			case INT_RED32: return GL_R32F;
			case INT_RG: return GL_RG8;
			case INT_RG32: return GL_RG32F;
			case INT_RGB: return GL_RGB8;
			case INT_RGB16:return GL_RGB16F;
			case INT_RGB32: return GL_RGB32F;
			case INT_RGBA: return GL_RGBA8;
			case INT_RGBA16: return GL_RGBA16F;
			case INT_RGBA32: return GL_RGBA32F;
			case INT_SRGB: return GL_SRGB8;
			case INT_SRGBA: return GL_SRGB8_ALPHA8;
			case INT_DEPTH: return GL_DEPTH_COMPONENT;
			case INT_DEPTH24: return GL_DEPTH_COMPONENT24;
			case INT_DEPTH32: return GL_DEPTH_COMPONENT32;
			case INT_DEPTHSTENCIL: return GL_DEPTH24_STENCIL8;
			case INT_DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			case INT_DEPTH32STENCIL8: return GL_DEPTH32F_STENCIL8;
		}
		LOG_WARN("[~gTexture~x] base internal format to stized internal format failed");
		return format;
	}

public:

	TextureParameters(TextureInternalFormat internalFormat = INT_RGBA, TextureDataFormat dataFormat = DATA_RGBA, TextureFilter filter = LINEAR, TextureWrap wrap = REPEAT, TextureType type = T_UNSIGNED_BYTE, bool flipY = true)
		: m_internalFormat(internalFormat), m_dataFormat(dataFormat), m_filter(filter), m_wrap(wrap), m_type(type), m_flipY(flipY) {
	}

	inline bool GetFlipY() const { return m_flipY; }
	inline int GetInternalFormat() const { return m_internalFormat; }
	inline int GetDataFormat() const { return m_dataFormat; }
	inline int GetWrap() const { return m_wrap; }
	inline int GetType() const { return m_type; }
	inline int GetFilter(int type, bool mipmap) const {
		switch (type) {
			case GL_TEXTURE_MIN_FILTER: {
				switch (m_filter) {
					//case LINEARMIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
					case LINEAR: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
					case NEAREST: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST;
				}
			}break;
			case GL_TEXTURE_MAG_FILTER: return m_filter == LINEAR ? GL_LINEAR : GL_NEAREST; break;
		}
		return GL_LINEAR;
	}
	inline int GetInternalFormatSized() {
		return BaseInternalFormatToSizedInternalFormat(m_internalFormat);
	}

	void SetDataFormat(TextureDataFormat dataFormat) {
		m_dataFormat = dataFormat;
	}

	void SetDataFormatFromChannelCount(int count) {
		switch (count) {
			case 1: SetDataFormat(DATA_RED); break;
			case 2: SetDataFormat(DATA_RG); break;
			case 3: SetDataFormat(DATA_RGB); break;
			case 4: SetDataFormat(DATA_RGBA); break;
		}
	}

	int GetChannelCount() {
		switch (m_dataFormat) {
			case DATA_RED: return 1;
			case DATA_RG: return 2;
			case DATA_RGB: return 3;
			case DATA_RGBA: return 4;
		}
		return 0;
	}


	String GetAsString() const {
		return Format("%s %s %s %s %s", InternalFormatToString(m_internalFormat).c_str(), DataFormatToString(m_dataFormat).c_str(), FilterToString(m_filter).c_str(), WrapToString(m_wrap).c_str(), TypeToString(m_type).c_str());
	}
};