#pragma once

class RenderingPipeline {
private:
	bool m_initialized = false;
	bool m_wireFrame = false;

	//Deferred
	GBuffer* m_gBuffer;
	AssetRef<Shader> m_directionalLightShader;
	AssetRef<Shader> m_pointLightShader;
	AssetRef<Shader> m_emissionShader;

	//SSAO
	bool m_ssaoEnabled = true;
	SSAORenderer* m_ssaoRenderer;

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

	ManagedRef<Camera> m_firstPersonCamera;
	ManagedRef<Camera> m_freeCam;
	ManagedRef<Camera> m_camera;
	int m_selectedCamera = 0;

	Matrix4 m_orthoMatrix;
	Matrix4 m_perspectiveMatrix;
	bool m_perspective = true;
	float m_lerpAmount = 0;
	int m_selectedTexture = 0;
	float m_gamma = 1;
	float m_exposure = 1;
	int m_selectedTonemapping = 11;

	uint m_width;
	uint m_height;
	bool m_bloom = true;
	float m_bloomFactor = 1.0f;
	float m_bloomMultiplier = 1.0f;
	float m_chromatic = 0;
public:
	DirectionalLight m_directionalLight;

	float roughness = 0;
	float metallic = 0;
	RenderingPipeline() {}
	~RenderingPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_ssaoRenderer);
	}

	void Initialize(uint width, uint height);

	void PreGeometryRender();
	void PostGeometryRender();
	void PreUIRender();
	void PostUIRender();
	void OnImGUI();
	void OnResize(uint width, uint height);

	inline GBuffer* GetGBuffer() { return m_gBuffer; }
	inline ManagedRef<Camera> GetCamera() { return m_camera; }
	inline bool Initialized() { return m_initialized; }
};