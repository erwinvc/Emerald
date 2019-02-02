#include "stdafx.h"


PointlightRenderer::~PointlightRenderer() {
    delete m_vao;
    delete m_ibo;
    delete m_pointlights;
}

PointlightRenderer::PointlightRenderer(int numvertices, int numindices, float* vertices, uint* indices) : m_numindices(numindices) {
    m_pointlights = new Pointlight[MAX_LIGHTS];

    for (int i = 0; i < MAX_LIGHTS; i++) {
        m_pointlights[i] = Pointlight();
    }
    //uniform float uLightRadius;
    //uniform vec3 uLightPosition;
    //uniform vec3 uLightColor;
    m_vao = new VertexArray();
    m_vao->AddBuffer(new Buffer(vertices, numvertices, 3), 0, false);
    m_pointlightBuffer = new Buffer((float*)m_pointlights, MAX_LIGHTS / 4 * sizeof(Pointlight), 8, GL_DYNAMIC_DRAW);
    m_vao->AddBuffer(m_pointlightBuffer, 1, true);

    m_ibo = new IndexBuffer(indices, numindices);


}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void PointlightRenderer::Draw(vector<Pointlight>& pointlights) {
    int count = Math::_max(pointlights.size(), MAX_LIGHTS);
    LOG("count %d", count);
    m_pointlightBuffer->Bind();
    GL(glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Pointlight), (float*)pointlights.data()));
    m_vao->Bind();

    m_pointlightBuffer->Bind();
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2*sizeof(Vector4), 0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4), BUFFER_OFFSET(16));
    glVertexAttribDivisor(2, 1);
    m_ibo->Bind();
    //GL(glDrawElementsInstanced(GL_TRIANGLES, m_numindices, GL_UNSIGNED_INT, 0, pointlights.size()));
    m_ibo->DrawInstanced(count);
}

