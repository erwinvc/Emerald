#pragma once

class LineRenderer : public Singleton<LineRenderer> {
private:
	struct Line {
		glm::vec3 m_begin;
		glm::vec3 m_end;
		Color m_color;

		Line(const float x1, const float y1, const float z1, const float x2, const  float y2, const  float z2, Color& color) : m_begin(x1, y1, z1), m_end(x2, y2, z2), m_color(color) {}
		Line(const glm::vec3 begin, const glm::vec3 end, Color& color) : m_begin(begin), m_end(end), m_color(color) {}
		Line() : m_begin(), m_end(), m_color() {}
	};

	InstancedRenderer<Line>* m_renderer;
	const uint MAX_OBJECTS = 256;
	AssetRef<Shader> m_shader;
	void Initialize();

	LineRenderer() { Initialize(); }
	~LineRenderer();

	friend Singleton;
public:
	void Begin();
	void Submit(glm::vec3 begin, glm::vec3 end, Color color);
	void Submit(float x1, float y1, float z1, float x2, float y2, float z2, Color& color);
	void End();
	void Draw();
	void DrawRect(const Rectangle& rect, Color& color, float y = 1.01f) {
		glm::vec4 c = rect.GetCornerPositions();
		Submit(c.x, y, c.y, c.z, y, c.y, color);
		Submit(c.z, y, c.y, c.z, y, c.w, color);
		Submit(c.z, y, c.w, c.x, y, c.w, color);
		Submit(c.x, y, c.w, c.x, y, c.y, color);
	}
	void DrawRectCentered(const Rectangle& rect, Color& color, float y = 1.01f) {
		glm::vec4 c = glm::vec4(rect.m_position.x, rect.m_position.y, rect.m_size.x, rect.m_size.y);
		Submit(c.x, y, c.y, c.z, y, c.y, color);
		Submit(c.z, y, c.y, c.z, y, c.w, color);
		Submit(c.z, y, c.w, c.x, y, c.w, color);
		Submit(c.x, y, c.w, c.x, y, c.y, color);
	}
};

static LineRenderer* GetLineRenderer() {
	return LineRenderer::GetInstance();
}