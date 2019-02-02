#pragma once

enum TextureWrap {
    CLAMP = 0x2900,
    REPEAT = 0x2901,
    MIRRORED_REPEAT = 0x8370,
    CLAMP_TO_EDGE = 0x812F,
    CLAMP_TO_BORDER = 0x812D
};

enum TextureFilter {
    LINEARMIPMAP,
    LINEAR,
    NEAREST
};

enum TextureFormat {
    RGBA = 0x8058,      //GL_RGBA8
    RGB = 0x8051,       //GL_RGB8
    RGBA32 = 0x8814,    //GL_RGBA32F
    RGB32 = 0x8815,     //GL_RGB32F
    RGBA16 = 0x881A,    //GL_RGBA16F
    RGB16 = 0x881B,     //GL_RGB16F
};

enum TextureType {
    T_UNSIGNED_BYTE = 0x1401,
    T_FLOAT = 0x1406,
};


class TextureParameters {
private:
    TextureFormat m_format;
    TextureFilter m_filter;
    TextureWrap m_wrap;
    TextureType m_type;
public:

    TextureParameters(TextureFormat format = RGBA, TextureFilter filter = LINEARMIPMAP, TextureWrap wrap = REPEAT, TextureType type = T_UNSIGNED_BYTE)
        : m_format(format), m_filter(filter), m_wrap(wrap), m_type(type) {
    };

    //TextureParameters(TextureFilter filter = LINEAR, TextureWrap wrap = CLAMP, TextureType type = T_UNSIGNED_BYTE)
    //    : m_format(RGBA), m_filter(filter), m_wrap(wrap), m_type(type) {
    //}
    //
    //TextureParameters(TextureWrap wrap = CLAMP, TextureType type = T_UNSIGNED_BYTE)
    //    : m_format(RGBA), m_filter(LINEAR), m_wrap(wrap), m_type(type) {
    //}
    //
    //TextureParameters(TextureType type = T_UNSIGNED_BYTE)
    //    : m_format(RGBA), m_filter(LINEAR), m_wrap(CLAMP), m_type(type) {
    //}

    inline int GetFormat() { return m_format; }
    inline int GetWrap() { return m_wrap; }
    inline int GetType() { return m_type; }
    inline int GetFilter(int type) {
        switch (type) {
        case GL_TEXTURE_MIN_FILTER: {
            switch (m_filter) {
            case LINEARMIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
            case LINEAR: return GL_LINEAR;
            case GL_NEAREST: return GL_NEAREST;
            }
        }break;
        case GL_TEXTURE_MAG_FILTER: return (m_filter == LINEAR || m_filter == LINEARMIPMAP) ? GL_LINEAR : GL_NEAREST; break;
        }
        return GL_LINEAR;
    }
};