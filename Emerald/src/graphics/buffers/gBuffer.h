#pragma once

class GBuffer {
private:
	AssetRef<FrameBuffer> m_fbo;
	uint m_depthRenderBuffer;
	uint m_width, m_height;

public:
	Texture* m_miscTexture;
	Texture* m_colorTexture;
	Texture* m_normalTexture;
	Texture* m_positionTexture;

	GBuffer(AssetRef<FrameBuffer> fbo, uint width, uint height);
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

	AssetRef<FrameBuffer> GetFBO() const { return m_fbo; }
};