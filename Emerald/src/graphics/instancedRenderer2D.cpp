#include "stdafx.h"

InstancedRenderer2D::~InstancedRenderer2D() {
    delete[] m_offsets;
    delete m_offsetsBuffer;
}

void InstancedRenderer2D::Initialize() {
    m_offsets = new Vector2[MAX_OBJECTS];

    m_offsetsBuffer = new Buffer((float*)m_offsets, MAX_OBJECTS * 2, 2, GL_DYNAMIC_DRAW);
    m_mesh->GetVAO()->AddBuffer(m_offsetsBuffer, 5, true);
}

void InstancedRenderer2D::Begin() {
    ASSERT(m_ended, "Call Renderer::End before calling Renderer::Begin");

    //m_offsetsPtr = m_offsets;
    m_amount = 0;
    m_ended = false;
    m_started = true;

    m_offsetsBuffer->Bind();
    GL(m_offsetsPtr = (Vector2*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void InstancedRenderer2D::Submit(Vector2& offset) {
    //shorten
    m_offsetsPtr->x = offset.x;
    m_offsetsPtr->y = offset.y;
    m_offsetsPtr++;
    m_amount++;
}

void InstancedRenderer2D::End() {
    ASSERT(m_started, "Call Renderer::Begin before calling Renderer::End");
    m_offsetsBuffer->Bind();
    GL(glUnmapBuffer(GL_ARRAY_BUFFER));
    //if (m_amount == 0) return;
    //m_offsetsBuffer->Bind();
    //glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(m_offsets), m_offsets);
    m_started = false;
    m_ended = true;
}
void InstancedRenderer2D::Draw(Shader* shader) {
    ASSERT(m_ended, "Call Renderer::End before calling Renderer::Draw");
    m_mesh->GetMaterial()->Bind(shader);
    m_mesh->DrawInstanced(m_amount);
    //glDrawElementsInstanced(GL_TRIANGLES, m_numindices, GL_UNSIGNED_INT, 0, m_amount);
}

