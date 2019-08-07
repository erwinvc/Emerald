#pragma once

class Texture;
class FrameBufferManager;

class FrameBuffer {
private:
	vector<AssetRef<Texture>> m_textures;
	String m_name;
	uint m_colorAttachments = 0;
	uint m_fbo = 0;
	uint m_dbo = 0;
	uint m_width, m_height;
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

	FrameBuffer(String name, uint width, uint height, Color& clearColor = Color::Black());
	~FrameBuffer();

	bool CheckStatus();

	friend FrameBufferManager;

public:
	AssetRef<Texture> AddColorBuffer(const TextureParameters& params);

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

	void Resize(uint width, uint height);

	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }
	inline void SetClearColor(Color& color) { m_color = color; }
};

class FrameBufferManager : public Singleton<FrameBufferManager> {
private:
	vector<FrameBuffer*> m_frameBuffers;
	FrameBufferManager() {}
	~FrameBufferManager() {}
	friend Singleton;

public:
	AssetRef<FrameBuffer> Create(const String& name, int width, int height) {
		//for (FrameBuffer* fbo : m_frameBuffers) {
		//	if (fbo->GetName().compare(name) == 0) {
		//		LOG_ERROR("[~cBuffers~x] ~rFramebuffer ~1%s~r already exists", fbo->GetName().c_str());
		//		return AssetRef<FrameBuffer>(fbo);
		//	}
		//}
		AssetRef<FrameBuffer> fbo = NEW(FrameBuffer(name, width, height));
		m_frameBuffers.push_back(fbo);
		return AssetRef<FrameBuffer>(fbo);
	}

	void Delete(AssetRef<FrameBuffer>& fbo) {
		Utils::RemoveFromVector(m_frameBuffers, fbo.Get());
	}
};

static FrameBufferManager* GetFrameBufferManager() { return FrameBufferManager::GetInstance(); }