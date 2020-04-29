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

	//HDR
	bool m_applyPostProcessing = true;
	bool m_FXAA = true;
	AssetRef<FrameBuffer> m_hdrBuffer;
	AssetRef<Texture> m_hdrTexture;
	AssetRef<Texture> m_hdrBrightTexture;
	AssetRef<Shader> m_hdrShader;
	AssetRef<Mesh> m_quad;

	//Bloom
	AssetRef<Shader> m_gaussianShader;
	AssetRef<FrameBuffer> m_pingPongFBO[2];
	AssetRef<Texture> m_pingPongTexture[2];

	AssetRef<OrthoCamera> m_uiCamera;
	AssetRef<FirstPersonCam> m_firstPersonCamera;
	AssetRef<FreeCam> m_freeCam;

	AssetRef<FrameBuffer> m_finalFBO;
	AssetRef<Texture> m_finalTexture;

	glm::mat4 m_orthoMatrix;
	glm::mat4 m_perspectiveMatrix;
	bool m_perspective = true;
	float m_lerpAmount = 0;
	int m_selectedTexture = 0;
	float m_gamma = 1;
	float m_exposure = 1;
	int m_selectedTonemapping = 11;

	bool m_bloom = true;
	float m_bloomFactor = 1.0f;
	float m_bloomMultiplier = 1.0f;

public:
	float m_ambientIntensity = 0.5f;

	DirectionalLight m_directionalLight;
	int m_selectedCamera = 0;
	float m_chromatic = 0;

	float roughness = 0;
	float metallic = 0;
	HDRPipeline() {}
	~HDRPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_ssaoRenderer);
		DELETE(m_ssrRenderer);
		DELETE(m_shadowRenderer);
		DELETE(m_spriteRenderer);
	}

	void EarlyInitialize(uint width, uint height) {
		m_width = width;
		m_height = height;
		m_uiCamera = NEW(OrthoCamera());
		m_uiCamera->SetViewport(0, 0, m_width, m_height);
		Camera::uiActive = m_uiCamera;
		m_spriteRenderer = NEW(SpriteRenderer());
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

	AssetRef<Texture> GetHDRTexture() { return m_hdrTexture; }
	AssetRef<Texture> GetFinalTexture() { return m_finalTexture; }

	inline void LineRect(Rectangle& rect, Color& color = Color::White(), float lineSize = 1.0f) {
		m_spriteRenderer->LineRect(rect, color, lineSize);
	}

	inline void Rect(glm::vec2 origin, float x, float y, float w, float h, float rotation = 0, const Color& color = Color::White(), Texture* texture = nullptr, const glm::vec3 atlasValues = glm::vec3(1.0f, 0.0f, 0.0f)) {
		m_spriteRenderer->Rect(origin, x, y, w, h, rotation, color, texture, atlasValues);
	}

	inline void Line(float x0, float y0, float x1, float y1, Color& color = Color::White(), float size = 1.0f) {
		m_spriteRenderer->Line(x0, y0, x1, y1, color, size);
	}
};