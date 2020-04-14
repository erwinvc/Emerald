#pragma once

class RenderingPipeline {
public:
	bool m_initialized = false;
	bool m_wireFrame = false;

	//Deferred
	GBuffer* m_gBuffer = nullptr;
	AssetRef<Shader> m_directionalLightShader;
	AssetRef<Shader> m_pointLightShader;
	AssetRef<Shader> m_emissionAmbientShader;

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

	AssetRef<FirstPersonCam> m_firstPersonCamera;
	AssetRef<FreeCam> m_freeCam;
	Camera* m_camera = nullptr;

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
	RenderingPipeline() {}
	~RenderingPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_ssaoRenderer);
	}

	void Initialize();

	void PreGeometryRender();
	void PostGeometryRender();
	void PreUIRender();
	void PostUIRender();
	void OnImGUI();
	void OnResize(uint width, uint height);

	inline GBuffer* GetGBuffer() { return m_gBuffer; }
	inline AssetRef<Camera> GetCamera() { return m_camera; }
	inline bool Initialized() { return m_initialized; }

	AssetRef<Texture> GetHDRTexture() { return m_hdrTexture; }
	AssetRef<Texture> GetFinalTexture() { return m_finalTexture; }
};