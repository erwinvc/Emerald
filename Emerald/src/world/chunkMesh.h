#pragma once

enum class BlockSide {
	DOWN,	// NEGATIVE Y
	UP,		// POSITIVE Y
	NORTH,	// NEGATIVE Z
	SOUTH,	// POSITIVE Z
	WEST,	// NEGATIVE X
	EAST	// POSITIVE X
};

static String BlockSideToString(BlockSide side) {
	switch (side) {
		case BlockSide::WEST: return "WEST";
		case BlockSide::EAST: return "EAST";
		case BlockSide::SOUTH: return "SOUTH";
		case BlockSide::NORTH: return "NORTH";
		case BlockSide::UP: return "UP";
		case BlockSide::DOWN: return "DOWN";
	}
	return "";
}

class ChunkMesh {
	struct Vertex {
		glm::uvec2 data;
		//glm::ivec3 m_position;
		//glm::vec3 m_index;
		//
		//Vertex(glm::ivec3 pos, glm::vec3 index) : m_position(pos), m_index(index) {}
		Vertex(glm::uvec2 dat) : data(dat) {}
	};

	vector<Vertex> m_vertices;
	vector<uint32> m_indices;
	int m_indexIndex = 0;
	bool m_generated = false;

public:
	Mesh* m_mesh;
	ChunkMesh() {
		m_vertices.reserve(8192);
		m_indices.reserve(12288);
	}
	~ChunkMesh() {
		if (m_generated) DELETE(m_mesh);
	}

	void AddFace(const bool face[12], BlockSide side, const glm::ivec3 position, uint8 block, uint8 xMinOffset, uint8 xMaxOffset, uint8 yMinOffset, uint8 yMaxOffset, uint8 zMinOffset, uint8 zMaxOffset);
	void GenerateMesh();
};