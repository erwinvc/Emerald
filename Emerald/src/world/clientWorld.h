#pragma once

class ClientWorld {
private:
	Material* m_chunkMaterial;
	Material* m_blockEntityMaterial;
	glm::vec3 m_chunkPosition;
	BlockState* m_hoveredBlock;
	BlockState* m_hoveredBlock2;
	BlockPos m_hoveredBlockPos;
	BlockPos m_hoveredBlockPos2;
	bool m_hasHoveredBlock;
	bool m_hasHoveredBlock2;
	Model* m_blockEntityModel;
	float m_rightClickDelayTimer = 0;
	float m_leftClickDelayTimer = 0;
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

	bool GetBlock(const WorldPos& blockPosition, BlockState*& blockState);
	bool GetBlock(const BlockPos& blockPosition, BlockState*& blockState);
	uint8 GetBlockID(const WorldPos& blockPosition) const;
	uint8 GetBlockID(const BlockPos& blockPosition) const;
	void SetChunkDirty(const ChunkPos& chunkPosition);
	void SetBlock(const WorldPos& blockPosition, uint8 voxel);
	void SetBlock(const BlockPos& blockPosition, uint8 voxel);
	void BreakBlock(const BlockPos& blockPosition, BlockSide blockSide);

	void AddEntity(uint32 id, const glm::vec3& position, String_t name, const glm::vec3& rotation, const glm::vec3& velocity);
	void UpdateEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& velocity);

	inline bool DoesChunkExist(const ChunkPos& pos) { return m_chunks.find(pos) != m_chunks.end(); }
	inline void SetTime(uint64 time) { m_time = time; }
	inline void Clear() { m_chunks.clear(); }
	inline void RemoveEntity(uint32 id) { m_entities[id].active = false; }
	inline uint64 GetTime() { return m_time; }

	vector<AABB> GetAABBs(const AABB& aabb);

	void RayTraceBlocks(glm::vec3 pos1, glm::vec3 pos2, bool returnLastUncollidableBlock);
};