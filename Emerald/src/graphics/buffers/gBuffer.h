#pragma once

class GBuffer {
private:
    FrameBuffer *m_fbo;
    uint m_depthRenderBuffer;
    uint m_width, m_height;
    void Initialize();

public:
    Texture* m_miscTexture;
    Texture* m_colorTexture;
    Texture* m_normalTexture;
    Texture* m_positionTexture;

    GBuffer(uint width, uint height) : m_width(width), m_height(height) { Initialize(); }
    GBuffer::~GBuffer() {
        delete m_fbo;
        delete m_miscTexture;
        delete m_colorTexture;
        delete m_normalTexture;
        delete m_positionTexture;
    }

    void BindTextures();

    void Bind() {
        m_fbo->Bind();
    }

    void Clear() {
        m_fbo->Clear();
    }

    void SetClearColor(Color& color) {
        m_fbo->SetClearColor(color);
    }

    void Unbind() {
        m_fbo->Unbind();
    }

	FrameBuffer* GetFBO() { return m_fbo; }
};

