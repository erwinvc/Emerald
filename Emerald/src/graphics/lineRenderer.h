#pragma once

class LineRenderer : public Singleton<LineRenderer> {
private:
	struct Line {
		Vector3 m_begin;
		Vector3 m_end;
		Color m_color;

		Line(float x1, float y1, float z1, float x2, float y2, float z2, Color& color) : m_begin(x1, y1, z1), m_end(x2, y2, z2), m_color(color) {}
		Line() : m_begin(), m_end(), m_color() {}
	};

	InstancedRenderer2D<Line>* m_renderer;
	const uint MAX_OBJECTS = 4096;
	AssetRef<Shader> m_shader;
	void Initialize();

	LineRenderer() { Initialize(); }
	~LineRenderer();

	friend Singleton;
public:
	void Begin();
	void Submit(Vector3& begin, Vector3& end, Color& color);
	void Submit(float x1, float y1, float z1, float x2, float y2, float z2, Color& color);
	void End();
	void Draw();
	void DrawRect(const Rect& rect, Color& color, float y = 1.01f) {
		Vector4 c = rect.GetCornerPositions();
		Submit(c.x, y, c.y, c.z, y, c.y, color);
		Submit(c.z, y, c.y, c.z, y, c.w, color);
		Submit(c.z, y, c.w, c.x, y, c.w, color);
		Submit(c.x, y, c.w, c.x, y, c.y, color);
	}
	void DrawRectCentered(const Rect& rect, Color& color, float y = 1.01f) {
		Vector4 c = Vector4(rect.m_position.x, rect.m_position.y, rect.m_size.x, rect.m_size.y);
		Submit(c.x, y, c.y, c.z, y, c.y, color);
		Submit(c.z, y, c.y, c.z, y, c.w, color);
		Submit(c.z, y, c.w, c.x, y, c.w, color);
		Submit(c.x, y, c.w, c.x, y, c.y, color);
	}
};

static LineRenderer* GetLineRenderer() {
	return LineRenderer::GetInstance();
}