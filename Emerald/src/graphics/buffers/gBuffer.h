#pragma once

class GBuffer {
private:
	AssetRef<FrameBuffer> m_fbo;
	uint m_width, m_height;

public:
	AssetRef<Texture> m_miscTexture;
	AssetRef<Texture> m_colorTexture;
	AssetRef<Texture> m_normalTexture;
	AssetRef<Texture> m_positionTexture;

	GBuffer(uint width, uint height);
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

	void Resize(uint width, uint height) {
		m_fbo->Resize(width, height);
	}

	AssetRef<FrameBuffer> GetFBO() const { return m_fbo; }
};