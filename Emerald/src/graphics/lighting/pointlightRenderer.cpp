#include "stdafx.h"

PointlightRenderer::PointlightRenderer(Mesh* mesh, uint32 maxLights) {
    ASSERT(maxLights <= MAX_LIGHTS, "[Rendering] Too many lights. Engine max is 32768")

	BufferLayout layout = {
	{ShaderDataType::Float4, "vars", 5, true},
	{ShaderDataType::Float4, "col", 6, true}
	};

	m_renderer = NEW(InstancedRenderer2D<Pointlight>(mesh, MAX_LIGHTS, layout));
}

void PointlightRenderer::Draw(vector<Pointlight>& pointlights) {
	m_renderer->Set(pointlights, pointlights.size());
	m_renderer->Draw();
}

