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

	//Deferred
	GBuffer* m_gBuffer = nullptr;
	AssetRef<Shader> m_directionalLightShader;
	AssetRef<Shader> m_pointLightShader;
	AssetRef<Shader> m_emissionAmbientShader;

	//Shadow
	AssetRef<ShadowRenderer> m_shadowRenderer;

	//SSAO
	SSAORenderer* m_ssaoRenderer = nullptr;
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


	AssetRef<OrthoCamera> m_uiCamera;
	AssetRef<FirstPersonCam> m_firstPersonCamera;
	AssetRef<FreeCam> m_freeCam;

	AssetRef<FrameBuffer> m_finalFBO;
	AssetRef<Texture> m_finalTexture;

	AssetRef<UniformBuffer<GlobalUniforms>> m_ubo;
	glm::mat4 m_orthoMatrix;
	glm::mat4 m_perspectiveMatrix;
	bool m_perspective = true;
	float m_lerpAmount = 0;
	int m_selectedTexture = 0;
	float m_gamma = 2.2f;
	float m_exposure = 1.0f;
	int m_selectedTonemapping = 12;

public:
	DepthCubemap* m_dcm;

	float m_ambientIntensity = 0.2f;

	DirectionalLight m_directionalLight;
	int m_selectedCamera = 0;

	float roughness = 0;
	float metallic = 0;
	HDRPipeline() {}
	~HDRPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_ssaoRenderer);
		DELETE(m_ssrRenderer);
		DELETE(m_shadowRenderer);
		DELETE(m_spriteRenderer);
		DELETE(m_lineRenderer);
	}

	void EarlyInitialize(uint width, uint height) {
		m_width = width;
		m_height = height;
		m_uiCamera = NEW(OrthoCamera());
		m_uiCamera->SetViewport(0, 0, m_width, m_height);
		Camera::uiActive = m_uiCamera;
		m_spriteRenderer = NEW(SpriteRenderer());
		m_ubo = NEW(UniformBuffer<GlobalUniforms>(GetShaderManager()->Get("Geometry")));
	}

	float Width() { return (float)m_width; }
	float Height() { return (float)m_height; }

	void Initialize();
	void Render();
	void PreShadowRender();
	void PostShadowRender();
	void PreGeometryRender();
	void PostGeometryRender();
	void OnImGUI();
	void OnResize(uint width, uint height);

	inline GBuffer* GetGBuffer() { return m_gBuffer; }
	inline bool Initialized() { return m_initialized; }

	Texture* GetHDRTexture() { return m_hdrTexture; }
	Texture* GetFinalTexture() { return m_finalTexture; }

	inline void UILineRect(Rectangle& rect, Color& color = Color::White(), float lineSize = 1.0f) {
		m_spriteRenderer->LineRect(rect, color, lineSize);
	}

	inline void UIRect(glm::vec2 origin, float x, float y, float w, float h, float rotation = 0, const Color& color = Color::White(), Texture* texture = nullptr, const glm::vec3 atlasValues = glm::vec3(1.0f, 0.0f, 0.0f)) {
		m_spriteRenderer->Rect(origin, x, y, w, h, rotation, color, texture, atlasValues);
	}

	inline void UILine(float x0, float y0, float x1, float y1, Color& color = Color::White(), float size = 1.0f) {
		m_spriteRenderer->Line(x0, y0, x1, y1, color, size);
	}

	inline void Bounds(glm::vec3 position, glm::vec3 size, Color color, bool overlay = false) {
		m_lineRenderer->Bounds(position, size, color, overlay);
	}
	
	inline void Line(glm::vec3 begin, glm::vec3 end, Color color, bool overlay = false) {
		m_lineRenderer->Line(begin, end, color, overlay);
	}
	
	inline void Line(float x0, float y0, float z0, float x1, float y1, float z1, Color& color, bool overlay = false) {
		m_lineRenderer->Line(x0, y0, z0, x1, y1, z1, color, overlay);
	}
};