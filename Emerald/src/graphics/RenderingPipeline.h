#pragma once

class RenderingPipeline {
private:
	bool m_initialized = false;
	//Deferred
	GBuffer* m_gBuffer;
	Shader* m_directionalLightShader;
	Shader* m_pointLightShader;
	DirectionalLight m_directionalLight;
	PointlightRenderer* m_pointlightRenderer;
	vector<Pointlight> m_pointlights;

	SSAORenderer* m_ssaoRenderer;

	//HDR
	bool m_applyPostProcessing = true;
	Ref<FrameBuffer> m_hdrBuffer;
	Texture* m_hdrTexture;
	Shader* m_hdrShader;
	Mesh* m_quad;

	Camera* m_camera;
	Matrix4 m_orthoMatrix;
	Matrix4 m_perspectiveMatrix;
	bool m_perspective = true;
	float m_lerpAmount = 0;
	int m_selectedTexture = 0;
	float m_gamma = 1;
	float m_exposure = 1;
	int m_selectedTonemapping = 8;
public:
	RenderingPipeline() {}
	~RenderingPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_pointlightRenderer);
		DELETE(m_ssaoRenderer);
		DELETE(m_quad);
	}

	void Initialize(int maxLights = PointlightRenderer::MAX_LIGHTS, int lightQuality = 20);

	void PreGeometryRender();
	void PostGeometryRender();
	void PreUIRender();
	void PostUIRender();
	void OnImGUI();
	void OnResize(uint width, uint height);

	inline GBuffer* GetGBuffer() { return m_gBuffer; }
	inline Camera* GetCamera() { return m_camera; }
	inline bool Initialized() { return m_initialized; }
	inline vector<Pointlight>& GetPointLights() { return m_pointlights; }
};