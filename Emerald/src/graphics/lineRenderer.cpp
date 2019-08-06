#include "stdafx.h"

LineRenderer::~LineRenderer() {
	delete[] m_lines;
}

void LineRenderer::Initialize() {
	m_lines = new Vector3[MAX_OBJECTS * 2];

	BufferLayout layout = {
		{ShaderDataType::Float3, "position", 0}
	};
	m_lineBuffer = NEW(VertexBuffer((float*)m_lines, MAX_OBJECTS * 2, layout, GL_DYNAMIC_DRAW));

	ManagedRef<VertexArray> vao(NEW(VertexArray()));
	vao->AddBuffer(m_lineBuffer);
	vao->ApplyLayouts();

	vector<uint> indices;
	indices.push_back(0);
	indices.push_back(1);

	ManagedRef<IndexBuffer> ibo(NEW(IndexBuffer(indices.data(), indices.size())));
	m_mesh = NEW(Mesh(vao, ibo));

	m_shader = GetShaderManager()->Get("Line");

	LOG("[~bRenderer~x] Line renderer initialized");
}

void LineRenderer::Begin() {
	m_amount = 0;
	m_ended = false;
	m_started = true;

	m_lineBuffer->Bind();
	GL(m_linesPtr = (Vector3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void LineRenderer::Submit(Vector3& begin, Vector3& end) {
	ASSERT(m_started, "Call LineRenderer::Begin before calling LineRenderer::Submit");
	m_linesPtr->x = begin.x;
	m_linesPtr->y = begin.y;
	m_linesPtr->z = begin.z;
	m_linesPtr++;
	m_linesPtr->x = end.x;
	m_linesPtr->y = end.y;
	m_linesPtr->z = end.z;
	m_linesPtr++;
	m_amount++;
}
void LineRenderer::Submit(float x1, float y1, float z1, float x2, float y2, float z2) {
	ASSERT(m_started, "Call LineRenderer::Begin before calling LineRenderer::Submit");
	m_linesPtr->x = x1;
	m_linesPtr->y = y1;
	m_linesPtr->z = z1;
	m_linesPtr++;
	m_linesPtr->x = x2;
	m_linesPtr->y = y2;
	m_linesPtr->z = z2;
	m_linesPtr++;
	m_amount++;
}

void LineRenderer::End() {
	ASSERT(m_started, "Call LineRenderer::Begin before calling LineRenderer::End");
	m_lineBuffer->Bind();
	GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	//if (m_amount == 0) return;
	//m_offsetsBuffer->Bind();
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_amount * sizeof(m_offsets), m_offsets);
	m_started = false;
	m_ended = true;
}
void LineRenderer::Draw() {
	ASSERT(m_ended, "Call LineRenderer::Begin & LineRenderer::End before calling LineRenderer::Draw");
	m_shader->Bind();
	m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
	m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());

	m_mesh->Bind();
	m_lineBuffer->Bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), 0);
	glVertexAttribDivisor(0, 1);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector3), BUFFEROFFSET(12));
	glVertexAttribDivisor(1, 1);

	m_mesh->GetIBO()->Bind();
	m_mesh->GetIBO()->DrawInstanced(m_amount, GL_LINES);
	m_lineBuffer->Unbind();
	m_mesh->Unbind();
	m_shader->Unbind();
}