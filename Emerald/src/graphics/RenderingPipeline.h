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
	GroundRaycast m_rayCast;
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

	void Initialize(int maxLights, int lightQuality);

public:
	RenderingPipeline(int maxLights = PointlightRenderer::MAX_LIGHTS, int lightQuality = 20) { Initialize(maxLights, lightQuality); }
	~RenderingPipeline() {
		DELETE(m_gBuffer);
		DELETE(m_geometryShader);
		DELETE(m_directionalLightShader);
		DELETE(m_pointLightShader);
		DELETE(m_pointlightRenderer);
		DELETE(m_tileRenderer);
		DELETE(m_ssaoRenderer);
		DELETE(m_hdrBuffer);
		DELETE(m_hdrShader);
		DELETE(m_quad);
		DELETE(m_uiShader);
	}

	void Update(const TimeStep& time);
	void Render();
	void RenderGeometry();
	void Resize(uint width, uint height);
	Camera* GetCamera() { return m_camera; }
};