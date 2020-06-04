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

	uint8 GetBlock(const WorldPos& blockPosition) const;
	void SetBlock(const WorldPos& blockPosition, uint8 voxel);
	
	void Update();

	uint32 AddEntity();
	EntityState& GetEntity(uint32 id);
	void RemoveEntity(uint id);
	void WriteEntities(PacketWriter& writer) const;

	inline unordered_map<glm::ivec3, Chunk>& GetChunks() { return m_chunks; };
	inline uint64 GetTime() { return m_time; }
};