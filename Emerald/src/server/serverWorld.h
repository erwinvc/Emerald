#pragma once

struct EntityState {
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };

	glm::vec3 lastPosition{ 0.0f };
	glm::vec3 lastRotation{ 0.0f };

	glm::vec3 velocity{ 0.0 };

	bool active = false;
};

class ServerWorld {
private:
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

	void SetChunkDirty(const ChunkPos& chunkPosition) {
		auto itr = m_chunks.find(chunkPosition);
		if (itr != m_chunks.cend()) {
			itr->second.m_dirty = true;
		}
	}

	void SetBlock(const WorldPos& blockPosition, uint8 voxel) {
		glm::vec3& pos = Camera::active->transform.position;
		AABB player(pos.x - 0.3f, pos.y - 1.62f, pos.z - 0.3f, pos.x + 0.3f, pos.y + 0.18f, pos.z + 0.3f);
		BlockPos blockPos = ToBlockPosition(blockPosition);
		AABB block(blockPos);
		if (player.Intersects(block)) return;

		auto chunkPosition = ToChunkPosition(blockPosition);
		auto itr = m_chunks.find(chunkPosition);
		auto local = ToLocalBlockPosition(blockPosition);
		if (itr != m_chunks.cend()) {
			itr->second.SetBlock(local, voxel);
			itr->second.m_dirty = true;
		}
	}

	uint32 AddEntity();
};