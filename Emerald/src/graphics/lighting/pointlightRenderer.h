#pragma once

class PointlightRenderer {
private:
    /*Max lights the engine can properly handle*/
    uint m_maxLights;
    Mesh* m_mesh;
    VertexBuffer* m_pointlightBuffer;
    int32 m_numindices;

    Pointlight* m_pointlights;

public:
    static const int32 MAX_LIGHTS = 32768;
    void Draw(vector<Pointlight>& pointlights);
    PointlightRenderer(Mesh* mesh, uint32 maxLights = MAX_LIGHTS);
    ~PointlightRenderer();
};