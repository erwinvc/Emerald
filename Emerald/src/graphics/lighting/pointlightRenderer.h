#pragma once

class PointlightRenderer {
private:
    const uint MAX_LIGHTS = 32768;
    VertexArray* m_vao;
    IndexBuffer* m_ibo;
    Buffer* m_pointlightBuffer;
    int m_numindices;

    Pointlight* m_pointlights;


public:
    void Draw(vector<Pointlight>& pointlights);

    PointlightRenderer::PointlightRenderer(int numvertices, int numindices, float* vertices, uint* indices);
    ~PointlightRenderer();
};