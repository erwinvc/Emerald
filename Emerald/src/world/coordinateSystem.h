#pragma once

bool BlockPositionOutOfChunkBounds(const glm::ivec3& position);
int ToLocalBlockIndex(const glm::ivec3& position);
glm::ivec3 ToChunkPosition(const glm::ivec3& position);
glm::ivec3 ToChunkPosition(const glm::vec3& position);
glm::ivec3 ToLocalBlockPosition(const glm::ivec3& position);
glm::ivec3 ToLocalBlockPosition(const glm::vec3& position);
BlockPos ToBlockPosition(const glm::vec3& vec);
glm::ivec3 ToGlobalBlockPosition(const glm::ivec3& blockPosition,const glm::ivec3& chunkPosition);