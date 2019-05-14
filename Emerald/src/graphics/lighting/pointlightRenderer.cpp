#include "stdafx.h"

//#Dirty using mesh to access vao & ibo?
PointlightRenderer::~PointlightRenderer() {
    delete m_mesh;
    delete m_pointlights;
}

PointlightRenderer::PointlightRenderer(Mesh* mesh, int maxLights) : m_mesh(mesh) {
    ASSERT(maxLights <= MAX_LIGHTS, "[Rendering] Too many lights. Engine max is 32768")
    m_pointlights = new Pointlight[MAX_LIGHTS];

    m_pointlightBuffer = new Buffer((float*)m_pointlights, MAX_LIGHTS / 4 * sizeof(Pointlight), 8, GL_DYNAMIC_DRAW);
    m_mesh->GetVAO()->AddBuffer(m_pointlightBuffer, 1, true);
}

void PointlightRenderer::Draw(vector<Pointlight>& pointlights) {

    //#Dirty create wrapper for this
    int count = Math::Max(pointlights.size(), MAX_LIGHTS);

    m_pointlightBuffer->Bind();
    GL(glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Pointlight), (float*)pointlights.data()));
    m_mesh->GetVAO()->Bind();

    m_pointlightBuffer->Bind();
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2*sizeof(Vector4), 0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4), BUFFEROFFSET(16));
    glVertexAttribDivisor(2, 1);
    m_mesh->GetIBO()->Bind();
    m_mesh->GetIBO()->DrawInstanced(count);
}

