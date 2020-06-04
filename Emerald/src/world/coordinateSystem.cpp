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

glm::ivec3 ToChunkPosition(const glm::ivec3& position) {
	int x = position.x;
	int y = position.y;
	int z = position.z;

	return {
		x >> 5,
		y >> 5,
		z >> 5,
	};
}

glm::ivec3 ToChunkPosition(const glm::vec3& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToChunkPosition(glm::ivec3{ x, y, z });
}

glm::ivec3 ToLocalBlockPosition(const glm::ivec3& position) {
	return { (CHUNK_SIZE + (position.x % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.y % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.z % CHUNK_SIZE)) % CHUNK_SIZE };
}

glm::ivec3 ToLocalBlockPosition(const glm::vec3& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToLocalBlockPosition(glm::ivec3{ x, y, z });
}

BlockPos ToBlockPosition(const glm::vec3& vec) {
	auto x = (int32)(Math::Floor(vec.x));
	auto y = (int32)(Math::Floor(vec.y));
	auto z = (int32)(Math::Floor(vec.z));
	return { x, y, z };
}

glm::ivec3 ToGlobalBlockPosition(const glm::ivec3& blockPosition,
	const glm::ivec3& chunkPosition) {
	return { chunkPosition.x * CHUNK_SIZE + blockPosition.x,
			chunkPosition.y * CHUNK_SIZE + blockPosition.y,
			chunkPosition.z * CHUNK_SIZE + blockPosition.z };
}