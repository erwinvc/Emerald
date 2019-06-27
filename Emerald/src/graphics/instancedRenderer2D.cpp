#include "stdafx.h"

InstancedRenderer2D::~InstancedRenderer2D() {
	delete[] m_offsets;
	delete m_offsetsBuffer;
}

void InstancedRenderer2D::Initialize() {
	m_offsets = new Vector2[MAX_OBJECTS];

	BufferLayout layout = {
		{ShaderDataType::Float2, "ehh", 5, true}
	};

	m_offsetsBuffer = NEW(VertexBuffer((float*)m_offsets, MAX_OBJECTS, layout, GL_DYNAMIC_DRAW));
	m_mesh->GetVAO()->AddBuffer(m_offsetsBuffer);
	m_mesh->GetVAO()->ApplyLayouts();
}

void InstancedRenderer2D::Begin() {
	ASSERT(m_ended, "Call InstancedRenderer2D::End before calling InstancedRenderer2D::Begin");

	//m_offsetsPtr = m_offsets;
	m_amount = 0;
	m_ended = false;
	m_started = true;

	m_offsetsBuffer->Bind();
	GL(m_offsetsPtr = (Vector2*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void InstancedRenderer2D::Submit(int x, int y) {
	if (m_amount >= MAX_OBJECTS) {
		LOG_ERROR("[~bEEngine~x] Max amount of tiles reached: %d", m_amount);
		return;
	}
	m_offsetsPtr->x = (float)x;
	m_offsetsPtr->y = (float)y;
	m_offsetsPtr++;
	m_amount++;
}

void InstancedRenderer2D::Submit(Vector2& offset) {
	//shorten
	m_offsetsPtr->x = offset.x;
	m_offsetsPtr->y = offset.y;
	m_offsetsPtr++;
	m_amount++;
}

void InstancedRenderer2D::End() {
	ASSERT(m_started, "Call InstancedRenderer2D::Begin before calling InstancedRenderer2D::End");
	m_offsetsBuffer->Bind();
	GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	//if (m_amount == 0) return;
	//m_offsetsBuffer->Bind();
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(m_offsets), m_offsets);
	m_started = false;
	m_ended = true;
}
void InstancedRenderer2D::Draw(Shader* shader) {
	ASSERT(m_ended, "Call InstancedRenderer2D::End before calling InstancedRenderer2D::Draw");
	m_mesh->GetMaterial()->Bind(shader);
	m_mesh->DrawInstanced(m_amount);
	//glDrawElementsInstanced(GL_TRIANGLES, m_numindices, GL_UNSIGNED_INT, 0, m_amount);
}

