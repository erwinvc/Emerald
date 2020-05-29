#pragma once

class ServerWorld {
private:
	uint32 m_entityCount = 0;
	vector<EntityState> m_entities;
	unordered_map<glm::ivec3, Chunk> m_chunks;

public:
	ServerWorld();

	uint8 GetBlock(const WorldPos& blockPosition) const {
		auto chunkPosition = ToChunkPosition(blockPosition);
		auto itr = m_chunks.find(chunkPosition);
		if (itr == m_chunks.cend()) {
			return 0;
		}
		return itr->second.GetBlockFast(ToLocalBlockPosition(blockPosition));
	}

	void SetBlock(const WorldPos& blockPosition, uint8 voxel) {
		auto chunkPosition = ToChunkPosition(blockPosition);
		auto itr = m_chunks.find(chunkPosition);
		auto local = ToLocalBlockPosition(blockPosition);
		if (itr != m_chunks.cend()) {
			itr->second.SetBlock(local, voxel);
		}
	}

	unordered_map<glm::ivec3, Chunk>& GetChunks() {
		return m_chunks;
	};


	uint32 AddEntity();
	void RemoveEntity(uint id) {
		assert(id < m_entities.size());
		m_entities[id].active = false;
		m_entityCount--;
	}

	EntityState& GetEntity(uint32 id) {
		return m_entities[id];
	}

	void WriteEntities(PacketWriter& writer) const {
		writer.Write(m_entityCount);
		for (int i = 0; i < m_entities.size(); i++) {
			const auto& state = m_entities[i];
			if (state.active) {
				writer.Write<uint32>(static_cast<uint32>(i));
				writer.Write<glm::vec3>(state.position);
				writer.Write<glm::vec3>(state.rotation);
			}
		}
	}
};