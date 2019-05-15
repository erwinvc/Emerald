#pragma once

class RenderingPipeline {
private:
	//Deferred
	GBuffer* m_gBuffer;
	Shader* m_geometryShader;
	Shader* m_directionalLightShader;
	Shader* m_pointLightShader;
	DirectionalLight m_directionalLight;
	PointlightRenderer* m_pointlightRenderer;
	vector<Pointlight> m_pointlights;
	TileRenderer* m_tileRenderer;

	SSAORenderer* m_ssaoRenderer;

	//HDR
	bool m_applyPostProcessing = true;
	FrameBuffer* m_hdrBuffer;
	Texture* m_hdrTexture;
	Shader* m_hdrShader;
	Mesh* m_quad;

	//UI
	UIShader* m_uiShader;

	Camera* m_camera;
	Matrix4 m_orthoMatrix;
	Matrix4 m_perspectiveMatrix;
	bool m_perspective = true;
	float m_lerpAmount = 0;

	World* m_world;
	void Initialize(int maxLights, int lightQuality);

public:
	RenderingPipeline(int maxLights = PointlightRenderer::MAX_LIGHTS, int lightQuality = 20) { Initialize(maxLights, lightQuality); }
	~RenderingPipeline() {
		delete m_gBuffer;
		delete m_geometryShader;
		delete m_directionalLightShader;
		delete m_pointLightShader;
		delete m_pointlightRenderer;
		delete m_tileRenderer;
		delete m_ssaoRenderer;
		delete m_hdrBuffer;
		delete m_hdrShader;
		delete m_quad;
		delete m_uiShader;
		delete m_world;
	}

	void Update(const TimeStep& time);
	void Render();
	void RenderGeometry();
	void Resize(uint width, uint height);
	Camera* GetCamera() { return m_camera; }
};