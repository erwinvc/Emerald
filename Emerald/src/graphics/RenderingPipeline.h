#pragma once

class RenderingPipeline {
private:
    //Deferred
    GBuffer* m_gBuffer;
    Shader* m_geometryShader;
    Shader* m_directionalLightShader;
    Shader* m_pointLightShader;
    PointlightRenderer* m_pointlightRenderer;
    vector<Pointlight> m_pointlights;

    //HDR
    bool m_applyPostProcessing;
    FrameBuffer* m_hdrBuffer;
    Shader* m_hdrShader;
    Mesh* m_quad;

    //UI
    UIShader* m_uiShader;

    Camera* m_camera;
    Matrix4 m_projectionMatrix;

    Vector3 m_directionalLight;

    void Initialize(int maxLights, int lightQuality);

public:
    RenderingPipeline(Camera* camera, int maxLights = PointlightRenderer::MAX_LIGHTS, int lightQuality = 20) : m_applyPostProcessing(true), m_camera(camera), m_directionalLight(Vector3(-0.7, 0.3, 0.1)) { Initialize(maxLights, lightQuality); }
    ~RenderingPipeline() {}

    void Render();
    void RenderGeometry();
    void Resize(uint width, uint height);
};