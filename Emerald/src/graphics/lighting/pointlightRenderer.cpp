#include "stdafx.h"

PointlightRenderer::~PointlightRenderer() {
	DELETE(m_renderer);
}

void PointlightRenderer::Initialize(Mesh* mesh, uint32 maxLights) {
	//if (maxLights > MAX_LIGHTS)LOG_ERROR("[Rendering] Too many lights. Engine max is 32768");

	BufferLayout layout = {
		{VertexBufferDataType::Float4, "vars", 1, true},
		{VertexBufferDataType::Float4, "col", 2, true}
	};

	m_renderer = NEW(InstancedRenderer<Pointlight>(mesh, MAX_LIGHTS, layout));
}

void PointlightRenderer::Begin() {
	m_renderer->Begin();
}

void PointlightRenderer::End() {
	m_renderer->End();
}
void PointlightRenderer::Draw() {
	m_renderer->Draw();
}

void PointlightRenderer::Submit(Pointlight* pointlights, int count) {
	m_renderer->Submit(pointlights, count, count);

}
void PointlightRenderer::Submit(Pointlight& pointlight) {
	m_renderer->Submit(pointlight);
}