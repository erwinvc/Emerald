#pragma once

class GBuffer {
private:
    FrameBuffer *m_fbo;
    uint m_depthRenderBuffer;
    uint m_width, m_height;

public:
    Texture* m_colorTexture;
    Texture* m_normalTexture;
    Texture* m_positionTexture;
    GBuffer(uint width, uint height) : m_width(width), m_height(height) {}
    GBuffer::~GBuffer() {
        delete m_fbo;
        delete m_colorTexture;
        delete m_normalTexture;
        delete m_positionTexture;
    }

    void Initialize();
    void BindTextures();

    void Bind() {
        m_fbo->Bind();
    }

    void Unbind() {
        m_fbo->Unbind();
    }
};

