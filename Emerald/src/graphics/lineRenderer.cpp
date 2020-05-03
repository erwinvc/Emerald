#include "stdafx.h"

LineRenderer::LineRenderer() {
	m_shader = GetShaderManager()->Get("Line");
	m_shader->Set("_Depth", 0);

	BufferLayout layout = {
		{VertexBufferDataType::Float3, "vsPos", 0, false},
		{VertexBufferDataType::Float4, "vsColor", 1, false}
	};

	uint32* indicesBuffer = new uint32[INDICES_SIZE];

	uint32 offset = 0;
	for (uint32 i = 0; i < INDICES_SIZE - 1; i += 2) {
		indicesBuffer[i] = offset++;
		indicesBuffer[i + 1] = offset++;
	}

	IndexBuffer* ibo = new IndexBuffer(indicesBuffer, INDICES_SIZE);
	Mesh* mesh = NEW(Mesh(NEW(VertexArray()), ibo, GetMaterialManager()->GetBasicMaterial(m_shader)));
	Mesh* mesh2 = NEW(Mesh(NEW(VertexArray()), ibo, GetMaterialManager()->GetBasicMaterial(m_shader)));

	m_overlayRenderer = NEW(ElementsRenderer<LineVertex>(mesh, VERTICES_COUNT, INDICES_COUNT, MAX_OBJECTS, layout));
	m_depthRenderer = NEW(ElementsRenderer<LineVertex>(mesh2, VERTICES_COUNT, INDICES_COUNT, MAX_OBJECTS, layout));

	DELETE(mesh);
	DELETE(mesh2);

	LOG("[~bRenderer~x] Line renderer initialized");
}
LineRenderer::~LineRenderer() {
	DELETE(m_overlayRenderer);
	DELETE(m_depthRenderer);
}

void LineRenderer::Begin() {
	m_overlayRenderer->Begin();
	m_depthRenderer->Begin();
}

void LineRenderer::Bounds(glm::vec3 position, glm::vec3 size, Color& color, bool overlay) {
	glm::vec3 x1 = (glm::vec3(1.0f, 1.0f, 1.0f) * size) + position;
	glm::vec3 x2 = (glm::vec3(-1.0f, 1.0f, 1.0f) * size) + position;
	glm::vec3 x3 = (glm::vec3(1.0f, 1.0f, -1.0f) * size) + position;
	glm::vec3 x4 = (glm::vec3(-1.0f, 1.0f, -1.0f) * size) + position;
	glm::vec3 x5 = (glm::vec3(1.0f, -1.0f, 1.0f) * size) + position;
	glm::vec3 x6 = (glm::vec3(-1.0f, -1.0f, 1.0f) * size) + position;
	glm::vec3 x7 = (glm::vec3(1.0f, -1.0f, -1.0f) * size) + position;
	glm::vec3 x8 = (glm::vec3(-1.0f, -1.0f, -1.0f) * size) + position;

	//Top
	Line(x1, x2, color);
	Line(x2, x4, color);
	Line(x4, x3, color);
	Line(x3, x1, color);

	//Bottom
	Line(x5, x6, color);
	Line(x6, x8, color);
	Line(x8, x7, color);
	Line(x7, x5, color);

	//Sides
	Line(x1, x5, color);
	Line(x2, x6, color);
	Line(x3, x7, color);
	Line(x4, x8, color);
}

void LineRenderer::Line(glm::vec3 begin, glm::vec3 end, Color color, bool overlay) {
	LineVertex vertices[2];
	vertices[0].m_pos = begin;
	vertices[0].m_color = color;
	vertices[1].m_pos = end;
	vertices[1].m_color = color;
	if (overlay) m_overlayRenderer->Submit(vertices, 2, 1);
	else m_depthRenderer->Submit(vertices, 2, 1);
}
void LineRenderer::Line(float x1, float y1, float z1, float x2, float y2, float z2, Color& color, bool overlay) {
	LineVertex vertices[2];
	vertices[0].m_pos = glm::vec3(x1, y1, z1);
	vertices[0].m_color = color;
	vertices[1].m_pos = glm::vec3(x2, y2, z2);
	vertices[1].m_color = color;
	if (overlay) m_overlayRenderer->Submit(vertices, 2, 1);
	else m_depthRenderer->Submit(vertices, 2, 1);
}

void LineRenderer::End() {
	m_overlayRenderer->End();
	m_depthRenderer->End();
}
void LineRenderer::Draw() {
	GLUtils::DisableDepthTest();
	m_overlayRenderer->Draw(GL_LINES);
	GLUtils::EnableDepthTest();
	m_depthRenderer->Draw(GL_LINES);
}