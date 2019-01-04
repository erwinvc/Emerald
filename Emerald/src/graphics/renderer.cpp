#include "stdafx.h"
#include "renderer.h"


Renderer::~Renderer() {
    delete m_vao;
    delete m_ibo;
    delete[] m_offsetsTempBuffer;
}

Renderer::Renderer(int numvertices, int numindices, float* vertices, float* normals, float* texcoords, float* tangents, uint* indices) : m_started(false), m_ended(false), m_amount(0), m_numindices(numindices), m_offsetsTempBuffer(nullptr), m_offsetsPtr(nullptr) {
    m_offsetsTempBuffer = new Vector3[MAX_OBJECTS];

    for (int i = 0; i < MAX_OBJECTS; i++) {
        m_offsetsTempBuffer[i].x = Math::RandomF(-1, 1);
        m_offsetsTempBuffer[i].y = Math::RandomF(-1, 1);
        m_offsetsTempBuffer[i].z = Math::RandomF(-1, 1);
    }

    m_vao = new VertexArray();
    m_vao->AddBuffer(new Buffer(vertices, numvertices * 3, 3), 0, false);
    m_vao->AddBuffer(new Buffer(texcoords, numvertices * 2, 2), 1, false);
    m_vao->AddBuffer(new Buffer(normals, numvertices * 3, 3), 2, false);
    m_vao->AddBuffer(new Buffer(tangents, numvertices * 3, 3), 3, false);
    m_offsets = new Buffer((float*)m_offsetsTempBuffer, MAX_OBJECTS * 3, 3, GL_DYNAMIC_DRAW);

    m_vao->AddBuffer(m_offsets, 4, true);

    m_ibo = new IndexBuffer(indices, numindices);

}

void Renderer::Begin() {
    m_offsetsPtr = m_offsetsTempBuffer;
    m_amount = 0;
    m_ended = false;
    m_started = true;
}

void Renderer::Submit(Vector3& offset) {
    m_offsetsPtr->x = offset.x;
    m_offsetsPtr->y = offset.y;
    m_offsetsPtr->z = offset.z;
    m_offsetsPtr++;
    m_amount++;
}

void Renderer::End() {
    ASSERT(m_started, "Call Renderer::Begin before calling Renderer::End");

    if (m_amount == 0) return;
    m_offsets->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(GLfloat) * 3, m_offsetsTempBuffer);
    m_started = false;
    m_ended = true;
}
void Renderer::Draw() {
    ASSERT(m_ended, "Call Renderer::End before calling Renderer::Draw");

    m_vao->Bind();
    m_ibo->Bind();
    glDrawElementsInstanced(GL_TRIANGLES, m_numindices, GL_UNSIGNED_INT, 0, m_amount);
    m_ibo->Draw(m_amount * m_numindices);
}

