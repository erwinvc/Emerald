#pragma once

class ServerWorld {
private:
	uint64 m_time = 0;
	uint32 m_timeCounter = 0;
	uint32 m_entityCount = 0;
	vector<EntityState> m_entities;
	unordered_map<glm::ivec3, Chunk> m_chunks;

public:
	ServerWorld();

	bool GetBlock(const WorldPos& blockPosition, BlockState*& blockState);
	bool GetBlock(const BlockPos& blockPosition, BlockState*& blockState);
	uint8 GetBlockID(const WorldPos& blockPosition) const;
	uint8 GetBlockID(const BlockPos& blockPosition) const;
	void SetBlock(const WorldPos& blockPosition, uint8 voxel);
	void SetBlock(const BlockPos& blockPosition, uint8 voxel);
	
	void Update();

	uint32 AddEntity();
	EntityState& GetEntity(uint32 id);
	void RemoveEntity(uint id);
	void WriteEntities(PacketWriter& writer) const;

	inline unordered_map<glm::ivec3, Chunk>& GetChunks() { return m_chunks; };
	inline uint64 GetTime() { return m_time; }
};