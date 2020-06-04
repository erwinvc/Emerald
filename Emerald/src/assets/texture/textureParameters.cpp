#include "stdafx.h"

String TextureParameters::WrapToString(TextureWrap wrap) const {
	switch (wrap) {
		case CLAMP: return "CLAMP";
		case REPEAT: return "REPEAT";
		case MIRRORED_REPEAT: return "MIRRORED_REPEAT";
		case CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
		case CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
	}
	return "NULL";
}

String TextureParameters::InternalFormatToString(TextureInternalFormat format) const {
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

String TextureParameters::DataFormatToString(TextureDataFormat format) const {
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

String TextureParameters::FilterToString(TextureFilter filter) const {
	switch (filter) {
		//case LINEARMIPMAP: return "LINEARMIPMAP";
		case LINEAR: return "LINEAR";
		case NEAREST: return "NEAREST";
	}
	return "NULL";
}

String TextureParameters::TypeToString(TextureType type) const {
	switch (type) {
		case T_UNSIGNED_BYTE: return "UNSIGNED_BYTE";
		case T_FLOAT: return "FLOAT";
		case T_UNSIGNED_INT_24_8: return "UNSIGNED_INT_24_8";
	}
	return "NULL";
}

int TextureParameters::BaseInternalFormatToSizedInternalFormat(TextureInternalFormat format) const {
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

int TextureParameters::GetInternalFormatSized() {
	return BaseInternalFormatToSizedInternalFormat(m_internalFormat);
}

int TextureParameters::GetFilter(int type, bool mipmap) const {
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

void TextureParameters::SetDataFormat(TextureDataFormat dataFormat) {
	m_dataFormat = dataFormat;
}

void TextureParameters::SetDataFormatFromChannelCount(int count) {
	switch (count) {
		case 1: SetDataFormat(DATA_RED); break;
		case 2: SetDataFormat(DATA_RG); break;
		case 3: SetDataFormat(DATA_RGB); break;
		case 4: SetDataFormat(DATA_RGBA); break;
	}
}

int TextureParameters::GetChannelCount() {
	switch (m_dataFormat) {
		case DATA_RED: return 1;
		case DATA_RG: return 2;
		case DATA_RGB: return 3;
		case DATA_RGBA: return 4;
	}
	return 0;
}

String TextureParameters::GetAsString() const {
	return Format("%s %s %s %s %s", InternalFormatToString(m_internalFormat).c_str(), DataFormatToString(m_dataFormat).c_str(), FilterToString(m_filter).c_str(), WrapToString(m_wrap).c_str(), TypeToString(m_type).c_str());
}