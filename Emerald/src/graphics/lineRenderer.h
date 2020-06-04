#pragma once

class LineRenderer {
private:
	struct LineVertex {
		glm::vec3 m_pos;
		Color m_color;

		LineVertex(const float x1, const float y1, const float z1, Color& color) : m_pos(glm::vec3(x1, y1, z1)), m_color(color) {}
		LineVertex(const glm::vec3 pos, Color& color) : m_pos(pos), m_color(color) {}
		LineVertex() : m_pos(glm::vec3(0.0f, 0.0f, 0.0f)), m_color(Color::White()) {}
	};

	ElementsRenderer<LineVertex>* m_depthRenderer;
	ElementsRenderer<LineVertex>* m_overlayRenderer;
	
	const uint MAX_OBJECTS = 512;
	const uint INDICES_COUNT = 2;
	const uint VERTICES_COUNT = 2;
	const uint INDICES_SIZE = MAX_OBJECTS * INDICES_COUNT;
	AssetRef<Shader> m_shader;

public:
	LineRenderer();
	~LineRenderer();

	void Begin();
	
	void Bounds(glm::vec3 position, glm::vec3 size, Color& color, bool overlay = false);
	void Line(glm::vec3 begin, glm::vec3 end, Color color, bool overlay = false);
	void Line(float x1, float y1, float z1, float x2, float y2, float z2, Color& color, bool overlay = false);
	void DrawRect(const Rectangle& rect, Color& color, float y = 1.01f);
	void DrawRectCentered(const Rectangle& rect, Color& color, float y = 1.01f);
	
	void End();
	void Draw();
};