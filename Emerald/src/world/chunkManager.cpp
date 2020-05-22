#include "stdafx.h"

//Chunk& ChunkManager::AddChunk(Chunk& chunk) {
//	auto itr = m_chunks.find(chunk);
//	if (itr == m_chunks.cend()) {
//		return m_chunks.emplace(chunk);
//	}
//	return itr->second;
//}
//
//const Chunk& ChunkManager::GetChunk(const glm::ivec3& chunk) {
//	auto itr = m_chunks.find(chunk);
//	if (itr == m_chunks.cend()) {
//		static Chunk errorChunk(*this, { 0, 0, 0 });
//		return errorChunk;
//	}
//	return itr->second;
//}
//
//voxel_t ChunkManager::GetVoxel(const glm::ivec3& voxelPosition) const {
//	auto chunkPosition = toChunkPosition(voxelPosition);
//	auto itr = m_chunks.find(chunkPosition);
//	if (itr == m_chunks.cend()) {
//		return 0;
//	}
//	return itr->second.qGetVoxel(toLocalVoxelPosition(voxelPosition));
//}
//
//void ChunkManager::SetVoxel(const glm::ivec3& voxelPosition, voxel_t voxel) {
//	auto chunkPosition = toChunkPosition(voxelPosition);
//	auto itr = m_chunks.find(chunkPosition);
//	auto local = toLocalVoxelPosition(voxelPosition);
//	if (itr != m_chunks.cend()) {
//		itr->second.qSetVoxel(local, voxel);
//	} else {
//		addChunk(chunkPosition).qSetVoxel(local, voxel);
//	}
//	ensureNeighbours(chunkPosition);
//}
//
//bool ChunkManager::HasChunk(const glm::ivec3& chunk) const {
//	return m_chunks.find(chunk) != m_chunks.cend();
//}
//
//bool ChunkManager::HasNeighbours(const glm::ivec3& chunkPosition) const {
//	const auto& cp = chunkPosition;
//	return hasChunk(chunkPosition) &&
//		// Top
//		hasChunk({ cp.x, cp.y + 1, cp.z }) &&
//		// Bottom
//		hasChunk({ cp.x, cp.y - 1, cp.z }) &&
//		// Left
//		hasChunk({ cp.x - 1, cp.y, cp.z }) &&
//		// Right
//		hasChunk({ cp.x + 1, cp.y, cp.z }) &&
//		// Front
//		hasChunk({ cp.x, cp.y, cp.z - 1 }) &&
//		// Back
//		hasChunk({ cp.x, cp.y, cp.z + 1 });
//}
//
//void ChunkManager::EnsureNeighbours(const glm::ivec3& chunkPosition) {
//	const auto& cp = chunkPosition;
//	AddChunk(cp);
//	AddChunk({ cp.x, cp.y + 1, cp.z });
//	AddChunk({ cp.x, cp.y - 1, cp.z });
//	AddChunk({ cp.x - 1, cp.y, cp.z });
//	AddChunk({ cp.x + 1, cp.y, cp.z });
//	AddChunk({ cp.x, cp.y, cp.z - 1 });
//	AddChunk({ cp.x, cp.y, cp.z + 1 });
//}
//
//const ChunkPositionMap<Chunk>& ChunkManager::chunks() const {
//	return m_chunks;
//}