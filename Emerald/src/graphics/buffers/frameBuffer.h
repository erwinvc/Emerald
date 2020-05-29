#pragma once

class Texture;
class FrameBufferManager;

enum class FBOAttachment {
	COLOR,
	DEPTH,
	STENCIL,
	DEPTHSTENCIL
};

enum FBOScale {
	STATIC,
	FULL,
	HALF,
	QUARTER,
	ONEEIGHTH
};

class FrameBuffer {
private:
	vector<AssetRef<Texture>> m_textures;
	vector<String> m_textureNames;
	FBOScale m_scale;
	String m_name;
	uint m_colorAttachments = 0;
	uint m_fbo = 0;
	uint m_width, m_height;
	uint m_realWidth, m_realHeight;
	Color m_color;
	bool m_hasDepth;
	bool m_hasStencil;

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

	FrameBuffer(String name, FBOScale scale, const Color& clearColor = Color::Black());
	FrameBuffer(String name, uint width, uint height, const Color& clearColor = Color::Black());
	~FrameBuffer();

	bool CheckStatus();

	friend FrameBufferManager;

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
			case FBOScale::ONEEIGHTH: return 0.125f;
		}
		return 1.0f;
	}

public:
	AssetRef<Texture> AddBuffer(const String& name, const TextureParameters& params, FBOAttachment type = FBOAttachment::COLOR);

	const String& GetName() { return m_name; }
	void Bind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
		GL(glViewport(0, 0, m_width, m_height));
	}
	void Unbind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void Clear() const {
		GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	}

	void ClearDepthOnly() const {
		GL(glClear(GL_DEPTH_BUFFER_BIT));
	}

	void ClearColorOnly() const {
		GL(glClear(GL_COLOR_BUFFER_BIT));
	}

	void ClearStencilOnly() const {
		GL(glClear(GL_STENCIL_BUFFER_BIT));
	}

	void SetDrawAndReadBuffersToNone() {
		Bind();
		GL(glDrawBuffer(GL_NONE));
		GL(glReadBuffer(GL_NONE));
		Unbind();
	}
	
	void Blit(FrameBuffer* targetFBO);
	void BlitDepthOnly(FrameBuffer* targetFBO);

	void Resize(uint width, uint height);

	uint GetWidth() const { return m_width; }
	uint GetHeight() const { return m_height; }
	FBOScale GetScale() const { return m_scale; }
	uint GetHandle() const { return m_fbo; }
	bool HasDepth() const { return m_hasDepth; }
	void SetClearColor(Color& color) { m_color = color; }
	vector<AssetRef<Texture>>& GetTextures() { return m_textures; }
	vector<String>& GetTextureNames() { return m_textureNames; }
	AssetRef<Texture> GetDepthTexture() { return m_textures[0]; }
};

class FrameBufferManager : public Singleton<FrameBufferManager> {
private:
	AssetRef<Texture> m_selectedTexture = nullptr;
	vector<FrameBuffer*> m_frameBuffers;
	FrameBufferManager() {}
	~FrameBufferManager() {}
	friend Singleton;

	uint m_width = 0;
	uint m_height = 0;
public:
	AssetRef<FrameBuffer> Create(const String& name, uint width, uint height, const Color& clearColor = Color::Black()) {
		for (FrameBuffer* fbo : m_frameBuffers) {
			if (fbo->GetName().compare(name) == 0) {
				LOG_ERROR("[~cBuffers~x] Framebuffer ~1%s~x already exists", fbo->GetName().c_str());
				return AssetRef<FrameBuffer>(fbo);
			}
		}
		AssetRef<FrameBuffer> fbo = NEW(FrameBuffer(name, width, height, clearColor));
		m_frameBuffers.push_back(fbo);
		return AssetRef<FrameBuffer>(fbo);
	}

	AssetRef<FrameBuffer> Create(const String& name, FBOScale scale, const Color& clearColor = Color::Black()) {
		for (FrameBuffer* fbo : m_frameBuffers) {
			if (fbo->GetName().compare(name) == 0) {
				LOG_ERROR("[~cBuffers~x] Framebuffer ~1%s~x already exists", fbo->GetName().c_str());
				return AssetRef<FrameBuffer>(fbo);
			}
		}
		AssetRef<FrameBuffer> fbo = NEW(FrameBuffer(name, scale, clearColor));
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
		if (m_width == width && m_height == height)return;
		m_width = width;
		m_height = height;
		for (FrameBuffer* fbo : m_frameBuffers) fbo->Resize(m_width, m_height);
	}
};

static FrameBufferManager* GetFrameBufferManager() { return FrameBufferManager::GetInstance(); }