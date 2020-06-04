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

	void SetScale(FBOScale scale);
	float FBOScaleToFloat(FBOScale scale);

public:
	AssetRef<Texture> AddBuffer(const String& name, const TextureParameters& params, FBOAttachment type = FBOAttachment::COLOR);

	const String& GetName() { return m_name; }
	
	void Bind() const;
	void Unbind() const;
	void Clear() const;
	void ClearDepthOnly() const;
	void ClearColorOnly() const;
	void ClearStencilOnly() const;
	void SetDrawAndReadBuffersToNone();
	
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
	AssetRef<FrameBuffer> Create(const String& name, uint width, uint height, const Color& clearColor = Color::Black());
	AssetRef<FrameBuffer> Create(const String& name, FBOScale scale, const Color& clearColor = Color::Black());

	void BindDefaultFBO();

	void OnResize(uint width, uint height);
	void OnImGUI();
	
	inline AssetRef<Texture> GetSelectedTexture() { return m_selectedTexture; }
	inline void SetSelectedTexture(Texture* texture) {	m_selectedTexture = texture; }
};

inline FrameBufferManager* GetFrameBufferManager() { return FrameBufferManager::GetInstance(); }