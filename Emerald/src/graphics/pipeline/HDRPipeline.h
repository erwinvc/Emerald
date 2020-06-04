#pragma once

class HDRPipeline {
public:
	bool m_initialized = false;
	bool m_wireFrame = false;
	bool m_ImGuiOpen = true;
	uint m_width = 0;
	uint m_height = 0;

	//UI
	AssetRef<SpriteRenderer> m_spriteRenderer;
	AssetRef<LineRenderer> m_lineRenderer;
	
	AssetRef<Shader> m_atmosphereShader;

	//Deferred
	GBuffer* m_gBuffer = nullptr;
	AssetRef<Shader> m_directionalLightShader;
	DirectionalShadow* m_directionalShadow;
	AssetRef<Shader> m_pointLightShader;
	AssetRef<Shader> m_emissionAmbientShader;
	
	//SSAO
	AmbientOcclusionRenderer* m_aoRenderer = nullptr;
	SSRRenderer* m_ssrRenderer = nullptr;

	//Bloom
	BloomRenderer* m_bloomRenderer = nullptr;
	
	//HDR
	bool m_applyPostProcessing = true;
	bool m_FXAA = true;
	AssetRef<FrameBuffer> m_hdrFBO;
	AssetRef<Texture> m_hdrTexture;
	AssetRef<Shader> m_hdrShader;
	AssetRef<Mesh> m_quad;
	AssetRef<Entity> m_cube;


	AssetRef<OrthoCamera> m_uiCamera;
	AssetRef<FirstPersonCam> m_firstPersonCamera;
	AssetRef<FreeCam> m_freeCam;

	AssetRef<FrameBuffer> m_finalFBO;
	AssetRef<Texture> m_finalTexture;

	AssetRef<UniformBuffer<GlobalUniforms>> m_ubo;
	
	float gamma = 2.2f;
	float exposure = 1.0f;
	int selectedTonemapping = 12;

	void PreGeometryRender();
	void PostGeometryRender();
	void OnImGUI();
	
public:
	float ambientIntensity = 0.05f;
	DirectionalLight directionalLight;
	
	HDRPipeline() {}
	~HDRPipeline();

	void EarlyInitialize(uint width, uint height);
	void Initialize();

	float Width() { return (float)m_width; }
	float Height() { return (float)m_height; }

	void Render(float partialUpdate);
	void RenderGeometry();
	void RenderGeometryShadow(ShadowType type);
	void OnResize(uint width, uint height);

	inline GBuffer* GetGBuffer() { return m_gBuffer; }
	inline bool Initialized() { return m_initialized; }

	inline Texture* GetHDRTexture() { return m_hdrTexture; }
	inline Texture* GetFinalTexture() { return m_finalTexture; }

	void UILineRect(Rectangle& rect, Color& color = Color::White(), float lineSize = 1.0f);
	void UIRect(glm::vec2 origin, float x, float y, float w, float h, float rotation = 0, const Color& color = Color::White(), Texture* texture = nullptr, const glm::vec3 atlasValues = glm::vec3(1.0f, 0.0f, 0.0f));
	void UILine(float x0, float y0, float x1, float y1, Color& color = Color::White(), float size = 1.0f);
	void Bounds(glm::vec3 position, glm::vec3 size, Color color, bool overlay = false);
	void Line(glm::vec3 begin, glm::vec3 end, Color color, bool overlay = false);
	void Line(float x0, float y0, float z0, float x1, float y1, float z1, Color& color, bool overlay = false);
};