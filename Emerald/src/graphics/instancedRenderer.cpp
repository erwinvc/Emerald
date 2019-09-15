#include "stdafx.h"

InstancedRenderer::~InstancedRenderer() {
	delete[] m_offsets;
}

void InstancedRenderer::Initialize() {
	m_offsets = NEWARRAY(Vector3, MAX_OBJECTS);

	BufferLayout layout = {
		{ShaderDataType::Float3, "offsets", 4}
	};

	m_offsetsBuffer = NEW(VertexBuffer((float*)m_offsets, MAX_OBJECTS, layout, GL_DYNAMIC_DRAW));
	m_mesh->GetVAO()->AddBuffer(m_offsetsBuffer);
	m_mesh->GetVAO()->ApplyLayouts();
}

void InstancedRenderer::Begin() {
	m_offsetsPtr = m_offsets;
	m_amount = 0;
	m_ended = false;
	m_started = true;
}

void InstancedRenderer::Submit(Vector3& offset) {
	//shorten
	m_offsetsPtr->x = offset.x;
	m_offsetsPtr->y = offset.y;
	m_offsetsPtr->z = offset.z;
	m_offsetsPtr++;
	m_amount++;
}

void InstancedRenderer::End() {
	ASSERT(m_started, "Call Renderer::Begin before calling Renderer::End");

	if (m_amount == 0) return;
	m_offsetsBuffer->Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(Vector3), m_offsets);
	m_started = false;
	m_ended = true;
}
void InstancedRenderer::Draw(uint mode) {
	ASSERT(m_ended, "Call Renderer::End before calling Renderer::Draw");
	m_mesh->Bind();
	//glDrawElementsInstanced(GL_TRIANGLES, m_numindices, GL_UNSIGNED_INT, 0, m_amount);
	m_mesh->DrawInstanced(m_amount, mode);
}

