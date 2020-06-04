#pragma once

class PointlightRenderer : public Singleton<PointlightRenderer> {
private:
	InstancedRenderer<Pointlight>* m_renderer;
	PointlightRenderer() {}
	~PointlightRenderer();
	friend Singleton;

public:
	static const int32 MAX_LIGHTS = 8196;
	void Initialize(Mesh* mesh, uint32 maxLights = MAX_LIGHTS);
	void Begin();
	void End();
	void Draw();
	void Submit(Pointlight* pointlights, int count);
	void Submit(Pointlight& pointlight);
};

inline PointlightRenderer* GetPointlightRenderer() { return PointlightRenderer::GetInstance(); }