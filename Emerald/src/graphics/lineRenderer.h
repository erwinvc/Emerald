#pragma once

class LineRenderer : public Singleton<LineRenderer> {
private:
	bool m_started;
	bool m_ended;
	const uint MAX_OBJECTS = 4096;
	Buffer* m_lineBuffer;
	int m_amount;
	Vector3* m_lines;
	Vector3* m_linesPtr;
	Mesh* m_mesh;
	Shader* m_shader;
	void Initialize();
public:
	void Begin();
	void Submit(Vector3& begin, Vector3& end);
	void Submit(float x1, float y1, float z1, float x2, float y2, float z2);
	void End();
	void Draw(Matrix4& projection, Matrix4& view);

	LineRenderer() { Initialize(); }
	~LineRenderer();
};

static LineRenderer* GetLineRenderer() {
	return &LineRenderer::GetInstance();
}