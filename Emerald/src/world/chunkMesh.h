#pragma once

enum BlockSide {
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
		glm::vec3 m_index;

		Vertex(glm::vec3 pos, glm::vec3 index) : m_position(pos), m_index(index) {}
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

	void AddFace(const float face[12], BlockSide side, const glm::ivec3 position, uint8 voxel);
	void GenerateMesh();
};