#pragma once

class GBuffer {
private:
	AssetRef<FrameBuffer> m_fbo;

public:
	AssetRef<Texture> m_attributesTexture;
	AssetRef<Texture> m_colorTexture;
	AssetRef<Texture> m_normalTexture;
	AssetRef<Texture> m_positionTexture;

	GBuffer();
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