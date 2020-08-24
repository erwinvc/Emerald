#pragma once

bool BlockPositionOutOfChunkBounds(const glm::ivec3& position);
int ToLocalBlockIndex(const glm::ivec3& position);

ChunkPos ToChunkPosition(const BlockPos& position);
ChunkPos ToChunkPosition(const WorldPos& position);
BlockPos ToLocalBlockPosition(const BlockPos& position);
BlockPos ToLocalBlockPosition(const WorldPos& position);
BlockPos ToBlockPosition(const glm::vec3& vec);
BlockPos ToGlobalBlockPosition(const BlockPos& blockPosition, const ChunkPos& chunkPosition);
