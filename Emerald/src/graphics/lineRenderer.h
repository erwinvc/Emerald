#pragma once

class LineRenderer : public Singleton<LineRenderer> {
private:
	bool m_started;
	bool m_ended;
	const uint MAX_OBJECTS = 4096;
	ManagedRef<VertexBuffer> m_lineBuffer;
	int m_amount;
	Vector3* m_lines;
	Vector3* m_linesPtr;
	AssetRef<Mesh> m_mesh;
	AssetRef<Shader> m_shader;
	void Initialize();

	LineRenderer() { Initialize(); }
	~LineRenderer();

	friend Singleton;
public:
	void Begin();
	void Submit(Vector3& begin, Vector3& end);
	void Submit(float x1, float y1, float z1, float x2, float y2, float z2);
	void End();
	void Draw();
	void DrawRect(const Rect& rect, float y = 1.01f) {
		Vector4& c = rect.GetCornerPositions();
		Submit(c.x, y, c.y, c.z, y, c.y);
		Submit(c.z, y, c.y, c.z, y, c.w);
		Submit(c.z, y, c.w, c.x, y, c.w);
		Submit(c.x, y, c.w, c.x, y, c.y);
	}
};

static LineRenderer* GetLineRenderer() {
	return LineRenderer::GetInstance();
}