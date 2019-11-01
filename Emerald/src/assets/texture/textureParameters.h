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

enum TextureFormat {
	RED = GL_RED,
	RG = GL_RG,
	RGBA = GL_RGBA,
	RGB = GL_RGB,
	RGBA32 = GL_RGBA32F,
	RGB32 = GL_RGB32F,
	RGBA16 = GL_RGBA16F,
	RGB16 = GL_RGB16F,
	DEPTH = GL_DEPTH_COMPONENT,
	DEPTH32 = GL_DEPTH_COMPONENT32
};

enum TextureType {
	T_UNSIGNED_BYTE = 0x1401,
	T_FLOAT = 0x1406,
};

class TextureParameters final {
private:
	TextureFormat m_internalFormat;
	TextureFormat m_format;
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

	String FormatToString(TextureFormat format) const {
		switch (format) {
			case RED: return "RED";
			case RG: return "RG";
			case RGBA: return "RGBA";
			case RGB: return "RGB";
			case RGBA32: return "RGBA32";
			case RGB32: return "RGB32";
			case RGBA16: return "RGBA16";
			case RGB16: return "RGB16";
			case DEPTH: return "DEPTH";
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
		}
		return "NULL";
	}

	int BaseInternalFormatToSizedInternalFormat(TextureFormat format) const {
		switch (format) {
			case RED: return GL_R8;
			case RG: return GL_RG8;
			case RGBA: return GL_RGBA8;
			case RGB: return GL_RGB8;
		}
		return format;
	}

public:

	TextureParameters(TextureFormat internalFormat = RGBA, TextureFormat format = RGBA, TextureFilter filter = LINEAR, TextureWrap wrap = REPEAT, TextureType type = T_UNSIGNED_BYTE, bool flipY = true)
		: m_internalFormat(internalFormat), m_format(format), m_filter(filter), m_wrap(wrap), m_type(type), m_flipY(flipY) {
	}

	inline bool GetFlipY() const { return m_flipY; }
	inline int GetInternalFormat() const { return m_internalFormat; }
	inline int GetFormat() const { return m_format; }
	inline int GetWrap() const { return m_wrap; }
	inline int GetType() const { return m_type; }
	inline int GetFilter(int type, bool mipmap) const {
		switch (type) {
			case GL_TEXTURE_MIN_FILTER: {
				switch (m_filter) {
					//case LINEARMIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
					case LINEAR: return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
					case NEAREST: return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
				}
			}break;
			case GL_TEXTURE_MAG_FILTER: return m_filter == LINEAR ? GL_LINEAR : GL_NEAREST; break;
		}
		return GL_LINEAR;
	}
	inline int GetInternalFormatSized() {
		return BaseInternalFormatToSizedInternalFormat(m_internalFormat);
	}

	void SetFormat(TextureFormat format) {
		m_format = format;
	}

	void SetFormatFromChannelCount(int count) {
		switch (count) {
			case 1: SetFormat(RED); break;
			case 2: SetFormat(RG); break;
			case 3: SetFormat(RGB); break;
			case 4: SetFormat(RGBA); break;
		}
	}

	int GetChannelCount() {
		switch (m_format) {
			case RED: return 1;
			case RG: return 2;
			case RGB: return 3;
			case RGBA: return 4;
		}
		return 0;
	}


	String GetAsString() const {
		return Format("%s %s %s %s %s", FormatToString(m_internalFormat).c_str(), FormatToString(m_format).c_str(), FilterToString(m_filter).c_str(), WrapToString(m_wrap).c_str(), TypeToString(m_type).c_str());
	}
};