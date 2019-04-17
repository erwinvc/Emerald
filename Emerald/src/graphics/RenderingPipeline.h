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

    Shader* m_ssaoShader;
	FrameBuffer* m_ssaoBuffer;
	Texture* m_ssaoTexture;
	Texture* m_ssaoNoiseTexture;

    //HDR
    bool m_applyPostProcessing = true;
    FrameBuffer* m_hdrBuffer;
	Texture* m_hdrTexture;
    Shader* m_hdrShader;
    Mesh* m_quad;

    //UI
    UIShader* m_uiShader;

    Camera* m_camera;
    Matrix4 m_projectionMatrix;

    void Initialize(int maxLights, int lightQuality);

public:
    RenderingPipeline(Camera* camera, int maxLights = PointlightRenderer::MAX_LIGHTS, int lightQuality = 20) : m_camera(camera) { Initialize(maxLights, lightQuality); }
    ~RenderingPipeline() {}

    void Update(const TimeStep& time);
    void Render();
    void RenderGeometry();
    void Resize(uint width, uint height);
};