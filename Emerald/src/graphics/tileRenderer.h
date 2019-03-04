#pragma once

class TileRenderer {
private:
    Shader* m_shader;
    InstancedRenderer2D* m_renderers[5];

    void Initialize();
public:
    TileRenderer() { Initialize(); }
    void Begin(Camera* cam, Matrix4& proj);
    void Submit(Tile& tile, Vector2& position);
    void Draw();
};