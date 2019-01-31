#pragma once

class Texture;

class FrameBuffer {
private:
    uint m_fbo;
    uint m_dbo;
    uint m_width, m_height;
    Color m_color;
    Texture* m_texture;
    void Initialize();

public:
    FrameBuffer(uint width, uint height) : m_fbo(0), m_dbo(0), m_width(width), m_height(height), m_color(Color(1, 0, 0)), m_texture(nullptr) { Initialize(); }
    ~FrameBuffer() { GL(glDeleteFramebuffers(1, &m_fbo)); }

    void Bind() {
        GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
        GL(glViewport(0, 0, m_width, m_height));
    }
    void Unbind() {
        GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
    void Clear() {
        GL(glClearColor(m_color.R, m_color.G, m_color.B, m_color.A));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    inline uint GetWidth() { return m_width; }
    inline uint GetHeight() { return m_height; }
    inline Texture* GetTexture() { return m_texture; }
    inline void SetColor(Color& color) { m_color = color; }
};