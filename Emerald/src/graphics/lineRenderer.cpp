#include "stdafx.h"

LineRenderer::~LineRenderer() {
}

void LineRenderer::Initialize() {
	//m_lines = new glm::vec3[MAX_OBJECTS * 2];

	BufferLayout layout = {
		{VertexBufferDataType::Float3, "vsBegin", 0, true},
		{VertexBufferDataType::Float3, "vsEnd", 1, true},
		{VertexBufferDataType::Float4, "vsColor", 2, true}
	};

	ManagedRef<VertexArray> vao(NEW(VertexArray()));
	vao->ApplyLayouts();

	vector<uint> indices;
	indices.push_back(0);
	indices.push_back(1);

	ManagedRef<IndexBuffer> ibo(NEW(IndexBuffer(indices.data(), indices.size())));
	Mesh* mesh = NEW(Mesh(vao, ibo));

	m_renderer = NEW(InstancedRenderer<Line>(mesh, MAX_OBJECTS, layout));

	m_shader = GetShaderManager()->Get("Line");

	LOG("[~bRenderer~x] Line renderer initialized");
}

void LineRenderer::Begin() {
	m_renderer->Begin();
}

void LineRenderer::Submit( glm::vec3 begin, glm::vec3 end, Color color) {
	m_renderer->Submit(Line(begin.x, begin.y, begin.z, end.x, end.y, end.z, color));
}
void LineRenderer::Submit(float x1, float y1, float z1, float x2, float y2, float z2, Color& color) {
	m_renderer->Submit(Line(x1, y1, z1, x2, y2, z2, color));
}


void LineRenderer::End() {
	m_renderer->End();
}
void LineRenderer::Draw() {
	m_shader->Bind();
	m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
	m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());
	m_renderer->Draw(GL_LINES);
}