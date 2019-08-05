#pragma once

class GBuffer {
private:
	Ref<FrameBuffer> m_fbo;
	uint m_depthRenderBuffer;
	uint m_width, m_height;

public:
	Texture* m_miscTexture;
	Texture* m_colorTexture;
	Texture* m_normalTexture;
	Texture* m_positionTexture;

	GBuffer(Ref<FrameBuffer> fbo, uint width, uint height);
	GBuffer::~GBuffer() {
	}

	void BindTextures();

	void Bind() {
		m_fbo->Bind();
	}

	void Clear() {
		m_fbo->Clear();
	}

	void Unbind() {
		m_fbo->Unbind();
	}

	Ref<FrameBuffer> GetFBO() const { return m_fbo; }
};