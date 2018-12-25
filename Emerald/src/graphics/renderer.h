#pragma once

struct Vertex {
	Vector3 m_position;
	Matrix4 m_transform;
	Color color;
};

class Renderer {
private:
	void Initialize();
	Vertex* m_buffer;
public:
	void Submit(Entity& entity);
	void Flush();

	Renderer();
	~Renderer();
};

