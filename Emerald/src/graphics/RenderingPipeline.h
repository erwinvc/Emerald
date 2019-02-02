#pragma once

class RenderingPipeline {
private:
    //Deferred
    GBuffer* m_gBuffer;
    Shader* m_geometryShader;
    Shader* m_directionalLightShader;
    Shader* m_pointLightShader;

    //HDR
    FrameBuffer* m_hdrBuffer;
    Shader* m_hdrShader;

    //UI
    UIShader* m_uiShader;

    Camera* m_camera;
    Matrix4 m_projectionMatrix;

    Vector3 m_directionalLight;

    void Initialize();

public:
    RenderingPipeline() : m_gBuffer(nullptr), m_hdrBuffer(nullptr), m_camera(nullptr), m_directionalLight(Vector3(-0.7, 0.3, 0.1)) { Initialize(); }
    ~RenderingPipeline() {}

    void Render();
    void RenderGeometry();
    void Resize(uint width, uint height);
};