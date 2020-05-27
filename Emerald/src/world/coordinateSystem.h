#pragma once

static bool BlockPositionOutOfChunkBounds(const glm::ivec3& position) {
	return
		position.x < 0 || position.x >= CHUNK_SIZE ||
		position.y < 0 || position.y >= CHUNK_SIZE ||
		position.z < 0 || position.z >= CHUNK_SIZE;
}

static int ToLocalBlockIndex(const glm::ivec3& position) {
	return position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
}

static glm::ivec3 ToChunkPosition(const glm::ivec3& position) {
	int x = position.x;
	int y = position.y;
	int z = position.z;
	//return {
	//	x < 0 ? ((x - CHUNK_SIZE + 1) / CHUNK_SIZE) : (x / CHUNK_SIZE),
	//	y < 0 ? ((y - CHUNK_SIZE + 1) / CHUNK_SIZE) : (y / CHUNK_SIZE),
	//	z < 0 ? ((z - CHUNK_SIZE + 1) / CHUNK_SIZE) : (z / CHUNK_SIZE),
	//};

	return {
		x >> 5,
		y >> 5,
		z >> 5,
	};
}

static glm::ivec3 ToChunkPosition(const glm::vec3& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToChunkPosition(glm::ivec3{ x, y, z });
}

static glm::ivec3 ToLocalBlockPosition(const glm::ivec3& position) {
	return { (CHUNK_SIZE + (position.x % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.y % CHUNK_SIZE)) % CHUNK_SIZE,
			(CHUNK_SIZE + (position.z % CHUNK_SIZE)) % CHUNK_SIZE };
}

static glm::ivec3 ToLocalBlockPosition(const glm::vec3& position) {
	int x = (int)Math::Floor(position.x);
	int y = (int)Math::Floor(position.y);
	int z = (int)Math::Floor(position.z);
	return ToLocalBlockPosition(glm::ivec3{ x, y, z });
}

static BlockPos ToBlockPosition(const glm::vec3& vec) {
	auto x = (int32)(Math::Floor(vec.x));
	auto y = (int32)(Math::Floor(vec.y));
	auto z = (int32)(Math::Floor(vec.z));
	return { x, y, z };
}

static glm::ivec3 ToGlobalBlockPosition(const glm::ivec3& blockPosition,
	const glm::ivec3& chunkPosition) {
	return { chunkPosition.x * CHUNK_SIZE + blockPosition.x,
			chunkPosition.y * CHUNK_SIZE + blockPosition.y,
			chunkPosition.z * CHUNK_SIZE + blockPosition.z };
}