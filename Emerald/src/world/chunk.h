#pragma once

class ServerWorld;
class Chunk {
private:
	uint8 m_blocks[CHUNK_VOLUME];
	ChunkMesh m_mesh;

public:
	bool m_dirty = true;
	glm::ivec3 m_position = glm::ivec3(0.0f, 0.0f, 0.0f);

	Chunk() {
		for (int i = 0; i < CHUNK_VOLUME; i++) m_blocks[i] = 1;
	}

	Chunk(glm::ivec3& pos, uint8 data[CHUNK_VOLUME]) : m_position(pos) {
		for (int i = 0; i < CHUNK_VOLUME; i++) m_blocks[i] = data[i];
	}
	
	Chunk(glm::ivec3& pos, bool filled = true) : m_position(pos) {
		for (int i = 0; i < CHUNK_VOLUME; i++) m_blocks[i] = filled ? 1 : 0;
	}


	void SetBlock(const glm::ivec3& position, uint8 block);

	uint8 GetBlockFast(const glm::ivec3& position) const;

	uint8 GetBlock(const glm::ivec3& position) const;

	void Draw() {
		if (m_dirty) GenerateMesh();
		m_mesh.m_mesh->Draw();
	}

	void GenerateMesh() {
		m_mesh = ChunkMeshGenerator::MakeChunkMesh(*this);
		m_mesh.GenerateMesh();
		m_dirty = false;
	}

	uint8 GetBlockAtIndex(uint32 index) {
		ASSERT(index >= 0 && index < CHUNK_VOLUME, "Out of chunk bounds");
		return m_blocks[index];
	}
};