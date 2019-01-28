#pragma once

class Mesh;

class UIShader {
private:
    Mesh* quad;
    Shader* m_shader;

    Matrix4 CreateMatrix(Vector2& origin, float rot, Vector2& position, Vector2& scale, bool flipX, bool flipY);
public:
    UIShader() : m_shader(new Shader("UI", "src/graphics/shaders/UI.vert", "src/graphics/shaders/UI.frag")) {}

    void Initialize();
    void RenderTexture(uint texture, Vector2& origin, float rot, Vector2& position, Vector2& size, Vector2& texSize, bool flipX, bool flipY);
};