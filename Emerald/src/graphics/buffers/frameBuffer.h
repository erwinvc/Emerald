#pragma once

class Texture;

class FrameBuffer {
private:
	String m_name;
	uint m_colorAttachments = 0;
    uint m_fbo = 0;
    uint m_dbo = 0;
    uint m_width, m_height;
    Color m_color;

	GLenum drawBuffers[16] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15
	};

    void Initialize();
	bool CheckStatus();
public:
    FrameBuffer(String name, uint width, uint height, Color& clearColor = Color(1, 0, 0)) : m_name(m_name), m_width(width), m_height(height), m_color(clearColor) { Initialize(); }
    ~FrameBuffer() {
        GL(glDeleteFramebuffers(1, &m_fbo));
    }

	void AddColorBuffer(Texture* texture);

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
    inline void SetClearColor(Color& color) { m_color = color; }
};