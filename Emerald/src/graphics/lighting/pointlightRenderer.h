#pragma once

class PointlightRenderer : public Singleton<PointlightRenderer> {
private:
	InstancedRenderer<Pointlight>* m_renderer;
	PointlightRenderer() {}
	~PointlightRenderer() {
		DELETE(m_renderer);
	}
	friend Singleton;

public:
	static const int32 MAX_LIGHTS = 8196;
	void Initialize(Mesh* mesh, uint32 maxLights = MAX_LIGHTS) {
		//if (maxLights > MAX_LIGHTS)LOG_ERROR("[Rendering] Too many lights. Engine max is 32768");

		BufferLayout layout = {
			{VertexBufferDataType::Float4, "vars", 1, true},
			{VertexBufferDataType::Float4, "col", 2, true}
		};

		m_renderer = NEW(InstancedRenderer<Pointlight>(mesh, MAX_LIGHTS, layout));
	}
	void Begin();
	void End();
	void Draw();
	void Submit(Pointlight* pointlights, int count);
	void Submit(Pointlight& pointlight);
};

static PointlightRenderer* GetPointlightRenderer() { return PointlightRenderer::GetInstance(); }