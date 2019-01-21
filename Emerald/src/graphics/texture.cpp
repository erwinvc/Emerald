#include "stdafx.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

Texture::Texture(const String& path) : m_textureID(0), m_width(0), m_height(0), m_path(path) {
    int width, height, bpc;

    stbi_set_flip_vertically_on_load(1);
    Byte* data = stbi_load(m_path.c_str(), &width, &height, &bpc, 4);
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float val;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Math::_min(16.0f, val));

    m_width = width;
    m_height = height;

    if (data) {
        stbi_image_free(data);
        LOG("[~yTexture~x] Loaded ~1%s", path.c_str());
    } else LOG("[~yTexture~x] Failed to load ~1%s", path.c_str());
}

Texture::~Texture() {
    glDeleteTextures(1, &m_textureID);
}

void Texture::Bind(uint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(uint slot) {
    glBindTexture(GL_TEXTURE_2D, slot);
}