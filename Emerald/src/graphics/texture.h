#pragma once
class Texture {
private:
    uint m_textureID;
    uint m_width, m_height;
    String m_path;

public:
    Texture(const String& path);
    ~Texture();
    void Bind(uint slot = 0);
    void Unbind();


    inline const uint GetWidth() const { return m_width; }
    inline const uint GetHeight() const { return m_height; }
};

