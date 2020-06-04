#pragma once

class ClientWorld {
private:
	Material* m_chunkMaterial;
	Material* m_blockEntityMaterial;
	glm::vec3 m_chunkPosition;
	glm::vec3 m_hoveredBlock;
	bool m_hasHoveredBlock;
	Model* m_blockEntityModel;
	float m_rightClickDelayTimer = 0;
	int m_selectedBlock = 1;
	uint64 m_time = 0;

public:
	vector<EntityState> m_entities;
	unordered_map<glm::ivec3, Chunk> m_chunks;

	ClientWorld();
	~ClientWorld();

	void Update(const TimeStep& time);
	void RenderChunks(Shader* shader);
	void RenderGeometry(HDRPipeline* pipeline);

	uint8 GetBlock(const glm::vec3& blockPosition) const;
	void SetChunkDirty(const ChunkPos& chunkPosition);
	void SetBlock(const glm::vec3& blockPosition, uint8 voxel);

	void AddEntity(uint32 id, const glm::vec3& position, String_t name, const glm::vec3& rotation, const glm::vec3& velocity);
	void UpdateEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& velocity);

	inline bool DoesChunkExist(const glm::ivec3& pos) { return m_chunks.find(pos) != m_chunks.end(); }
	inline void SetTime(uint64 time) { m_time = time; }
	inline void Clear() { m_chunks.clear(); }
	inline void RemoveEntity(uint32 id) { m_entities[id].active = false; }
	inline uint64 GetTime() { return m_time; }
};