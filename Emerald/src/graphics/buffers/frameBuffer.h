#pragma once

class Texture;

class FrameBuffer {
private:
    uint m_fbo;
    uint m_dbo;
    uint m_width, m_height;
    bool m_multisampled;
    Color m_color;
    Texture* m_texture;
    void Initialize();

public:
    FrameBuffer(uint width, uint height, bool multisampled = false, Color& clearColor = Color(1, 0, 0)) : m_fbo(0), m_dbo(0), m_width(width), m_height(height), m_multisampled(multisampled), m_color(clearColor), m_texture(nullptr) { Initialize(); }
    ~FrameBuffer() {
        GL(glDeleteFramebuffers(1, &m_fbo));
        delete m_texture;
    }

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
    inline void SetClearColor(Color& color) { m_color = color; }
};