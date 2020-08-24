#include "stdafx.h"

bool BlockPositionOutOfChunkBounds(const glm::ivec3& position) {
	return
		position.x < 0 || position.x >= CHUNK_SIZE ||
		position.y < 0 || position.y >= CHUNK_SIZE ||
		position.z < 0 || position.z >= CHUNK_SIZE;
}

int ToLocalBlockIndex(const glm::ivec3& position) {
	return position.y * (CHUNK_AREA)+position.z * CHUNK_SIZE + position.x;
}

ChunkPos ToChunkPosition(const BlockPos& position) {
	return {
		position.x >> 5,
		position.y >> 5,
		position.z >> 5,
	};
}

ChunkPos ToChunkPosition(const WorldPos& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToChunkPosition(BlockPos{ x, y, z });
}

BlockPos ToLocalBlockPosition(const BlockPos& position) {
	return { (CHUNK_SIZE + (position.x % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.y % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.z % CHUNK_SIZE)) % CHUNK_SIZE };
}

BlockPos ToLocalBlockPosition(const WorldPos& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToLocalBlockPosition(BlockPos{ x, y, z });
}

BlockPos ToBlockPosition(const glm::vec3& vec) {
	auto x = (int32)(Math::Floor(vec.x));
	auto y = (int32)(Math::Floor(vec.y));
	auto z = (int32)(Math::Floor(vec.z));
	return { x, y, z };
}

BlockPos ToGlobalBlockPosition(const BlockPos& blockPosition, const ChunkPos& chunkPosition) {
	return { chunkPosition.x * CHUNK_SIZE + blockPosition.x,
			chunkPosition.y * CHUNK_SIZE + blockPosition.y,
			chunkPosition.z * CHUNK_SIZE + blockPosition.z };
}