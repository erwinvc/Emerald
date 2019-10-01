#pragma once

class Texture;
class FrameBufferManager;

enum class FBOScale {
	FULL,
	HALF,
	QUARTER
};

class FrameBuffer {
private:
	vector<AssetRef<Texture>> m_textures;
	vector<String> m_textureNames;
	FBOScale m_scale;
	String m_name;
	uint m_colorAttachments = 0;
	uint m_fbo = 0;
	uint m_dbo = 0;
	uint m_width, m_height;
	uint m_realWidth, m_realHeight;
	Color m_color;

	const GLenum drawBuffers[16] = {
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

	FrameBuffer(String name, FBOScale scale, Color& clearColor = Color::Black());
	~FrameBuffer();

	bool CheckStatus();

	friend FrameBufferManager;
	void Resize(uint width, uint height);

	void SetScale(FBOScale scale) {
		if (m_scale == scale) return;
		m_scale = scale;
		Resize(m_realWidth, m_realHeight);
	}

	float FBOScaleToFloat(FBOScale scale) {
		switch (scale) {
		case FBOScale::FULL: return 1.0f;
		case FBOScale::HALF: return 0.5f;
		case FBOScale::QUARTER: return 0.25f;
		}
		return 1.0f;
	}

public:
	AssetRef<Texture> AddColorBuffer(const String& name, const TextureParameters& params);

	const String& GetName() { return m_name; }
	void Bind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
		GL(glViewport(0, 0, m_width, m_height));
	}
	void Unbind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void Clear() const {
		GL(glClearColor(m_color.R, m_color.G, m_color.B, m_color.A));
		GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}


	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }
	inline FBOScale GetScale() const { return m_scale; }
	inline uint GetHandle() const { return m_fbo; }
	inline void SetClearColor(Color& color) { m_color = color; }
	inline vector<AssetRef<Texture>>& GetTextures() { return m_textures; }
	inline vector<String>& GetTextureNames() { return m_textureNames; }
};

class FrameBufferManager : public Singleton<FrameBufferManager> {
private:
	AssetRef<Texture> m_selectedTexture = nullptr;
	vector<FrameBuffer*> m_frameBuffers;
	FrameBufferManager() {}
	~FrameBufferManager() {}
	friend Singleton;

public:
	AssetRef<FrameBuffer> Create(const String& name, FBOScale scale) {
		for (FrameBuffer* fbo : m_frameBuffers) {
			if (fbo->GetName().compare(name) == 0) {
				LOG_ERROR("[~cBuffers~x] ~rFramebuffer ~1%s~r already exists", fbo->GetName().c_str());
				return AssetRef<FrameBuffer>(fbo);
			}
		}
		AssetRef<FrameBuffer> fbo = NEW(FrameBuffer(name, scale));
		m_frameBuffers.push_back(fbo);
		return AssetRef<FrameBuffer>(fbo);
	}

	void BindDefaultFBO();

	void Delete(AssetRef<FrameBuffer>& fbo) {
		Utils::RemoveFromVector(m_frameBuffers, fbo.Get());
	}

	void OnImGUI();
	inline AssetRef<Texture> GetSelectedTexture() { return m_selectedTexture; }

	void SetSelectedTexture(AssetRef<Texture> texture) {
		m_selectedTexture = texture;
	}

	void OnResize(uint width, uint height) {
		for (FrameBuffer* fbo : m_frameBuffers) fbo->Resize(width, height);
	}
};

static FrameBufferManager* GetFrameBufferManager() { return FrameBufferManager::GetInstance(); }