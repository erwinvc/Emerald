#pragma once

enum BlockSide
{
	LEFT,
	RIGHT,
	FRONT,
	BACK,
	TOP,
	BOTTOM
};

class ChunkMesh {
	struct Vertex {
		glm::vec3 m_position;
		glm::vec2 m_index;

		Vertex(glm::vec3 pos, glm::vec2 index) : m_position(pos), m_index(index) {}
	};

	vector<Vertex> m_vertices;
	vector<uint> m_indices;
	int m_indexIndex = 0;
	bool m_generated = false;

public:
	Mesh* m_mesh;
	~ChunkMesh() {
		if (m_generated) DELETE(m_mesh);
	}

	void AddFace(const float face[12], BlockSide side, const glm::ivec3 position) {
		int index = 0;
		for (unsigned i = 0; i < 4; i++) {
			float x = face[index++] + position.x;
			float y = face[index++] + position.y;
			float z = face[index++] + position.z;

			m_vertices.emplace_back(glm::vec3(x, y, z), glm::vec2(i, (int)side));
		}

		m_indices.push_back(m_indexIndex);
		m_indices.push_back(m_indexIndex + 1);
		m_indices.push_back(m_indexIndex + 2);
		m_indices.push_back(m_indexIndex + 2);
		m_indices.push_back(m_indexIndex + 3);
		m_indices.push_back(m_indexIndex);
		m_indexIndex += 4;
	}

	void GenerateMesh() {
		ManagedRef<VertexArray> vao(new VertexArray());
		ManagedRef<IndexBuffer> ibo(new IndexBuffer(m_indices.data(), m_indices.size()));

		BufferLayout layout = {
			{VertexBufferDataType::Float3, "vsPos", 0},
			{VertexBufferDataType::Float2, "vsIndex", 1}
		};

		vao->AddBuffer(NEW(VertexBuffer(m_vertices.data(), m_vertices.size(), layout)));
		vao->ApplyLayouts();
		m_mesh = NEW(Mesh(vao, ibo));
		m_generated = true;
	}

};