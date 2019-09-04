#pragma once

class RenderingPipeline {
private:
	bool m_initialized = false;

	//Deferred
	GBuffer* m_gBuffer;
	AssetRef<Shader> m_directionalLightShader;
	AssetRef<Shader> m_pointLightShader;
	DirectionalLight m_directionalLight;

	//SSAO
	bool m_ssaoEnabled = false;
	SSAORenderer* m_ssaoRenderer;

	//HDR
	bool m_applyPostProcessing = true;
	bool m_FXAA = true;
	AssetRef<FrameBuffer> m_hdrBuffer;
	AssetRef<Texture> m_hdrTexture;
	//AssetRef<Texture> m_hdrBrightTexture;
	AssetRef<Shader> m_hdrShader;
	AssetRef<Mesh> m_quad;

	//Bloom
	//Ref<Shader> m_gaussianShader;
	//Ref<FrameBuffer> m_pingPongFBO[2];
	//Ref<Texture> m_pingPongTexture[2];

	ManagedRef<Camera> m_camera;
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
	//bool m_bloom = true;
public:
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