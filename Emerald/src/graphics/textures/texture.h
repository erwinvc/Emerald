#pragma once

class Texture {
private:
    TextureParameters m_params;
    uint m_textureID;
    uint m_width, m_height;
    String m_path;

    void Load(byte* data);
public:
    Texture(uint width, uint height, byte* data, TextureParameters params = TextureParameters());
    Texture(uint width, uint height, TextureParameters params = TextureParameters());
    Texture(const String& path, TextureParameters params = TextureParameters());
    Texture(uint textureID, uint width, uint height, TextureParameters params = TextureParameters()) : m_params(params), m_textureID(textureID), m_width(width), m_height(height) {};
    ~Texture();

    uint GetHandle() { return m_textureID; }
    void Bind(uint slot = 0);
    void Unbind(uint slot = 0);


    inline const uint GetWidth() const { return m_width; }
    inline const uint GetHeight() const { return m_height; }
};