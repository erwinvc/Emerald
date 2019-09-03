#pragma once

class CustomMesh : public Mesh {
private:
	Vector3* m_vertices;
	uint m_vertexCount;

	const BufferLayout m_layout = {
	{ShaderDataType::Float3, "vsPos", 0},
	{ShaderDataType::Float3, "vsNormal", 0},
	{ShaderDataType::Float2, "vsUv", 0},
	};

public:
	CustomMesh() : Mesh() {
		m_vao = NEW(VertexArray());
		m_vao->AddBuffer(NEW(VertexBuffer((float*)nullptr, 0, m_layout)));
		m_vao->ApplyLayouts();

		m_ibo = NEW(IndexBuffer(nullptr, 0));

	}

	void SetVertices(Vertex* vertices, uint count) {
	}
};