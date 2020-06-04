#pragma once

class GBuffer {
private:
	AssetRef<FrameBuffer> m_fbo;

public:
	AssetRef<Texture> m_attributesTexture;
	AssetRef<Texture> m_colorTexture;
	AssetRef<Texture> m_normalTexture;

	GBuffer();
	GBuffer::~GBuffer() {}

	void BindTextures();

	void Bind();
	void Clear();
	void Unbind();

	inline FrameBuffer* GetFBO() const { return m_fbo.Get(); }
};