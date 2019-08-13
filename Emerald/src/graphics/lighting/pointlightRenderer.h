#pragma once

class PointlightRenderer {
private:
	InstancedRenderer2D<Pointlight>* m_renderer;

public:
	static const int32 MAX_LIGHTS = 32768;
	void Draw(vector<Pointlight>& pointlights);
	PointlightRenderer(Mesh* mesh, uint32 maxLights = MAX_LIGHTS);
	~PointlightRenderer() {
		DELETE(m_renderer);
	}
};