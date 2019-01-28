#pragma once

class GBuffer {
private:
    uint m_fbo;

    uint m_depthRenderBuffer;
    uint m_width, m_height;

    void CreateTexture(uint& tex, int format, int type, int pos) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, GL_RGBA, type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + pos, GL_TEXTURE_2D, tex, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
public:
    uint m_colorTexture;
    uint m_normalTexture;
    uint m_positionTexture;
    GBuffer(uint width, uint height) : m_width(width), m_height(height) {}
    GBuffer::~GBuffer() {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_colorTexture);
        glDeleteTextures(1, &m_normalTexture);
        glDeleteTextures(1, &m_positionTexture);
        glDeleteTextures(1, &m_depthRenderBuffer);
    }


    void Initialize() {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        CreateTexture(m_colorTexture, GL_RGBA8, GL_UNSIGNED_BYTE, 0);
        CreateTexture(m_normalTexture, GL_RGBA16F, GL_FLOAT, 1);
        CreateTexture(m_positionTexture, GL_RGB32F, GL_FLOAT, 2);

        glGenRenderbuffers(1, &m_depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_width, m_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

        GLenum drawBuffers[3] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
        };

        glDrawBuffers(3, drawBuffers);

        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[Buffers] initializing GBuffer failed %d", status);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Bind() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    }

    void BindTextures() {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, m_normalTexture);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, m_positionTexture);
    }

    void Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

