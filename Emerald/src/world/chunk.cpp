#include "stdafx.h"
#include "gzip/compress.hpp"
#include "gzip/decompress.hpp"

uint8 Chunk::GetBlock(const glm::ivec3& position) const {
	if (BlockPositionOutOfChunkBounds(position)) {
		if (GetApp()->IsServer()) {
			ServerWorld* m_world = GetServer()->GetWorld();
			return m_world->GetBlock(ToGlobalBlockPosition(position, m_position));
		} else {
			ClientWorld* m_world = GameStates::VOXEL->GetWorld();
			return m_world->GetBlock(ToGlobalBlockPosition(position, m_position));
		}
	}
	return m_blocks[ToLocalBlockIndex(position)];
}


void Chunk::SetBlock(const glm::ivec3& position, uint8 block) {
	if (!GetApp()->IsServer()) {
		ClientWorld* m_world = GameStates::VOXEL->GetWorld();
		if (position.x == 31) m_world->SetChunkDirty(m_position + glm::ivec3(1, 0, 0));
		if (position.y == 31) m_world->SetChunkDirty(m_position + glm::ivec3(0, 1, 0));
		if (position.z == 31) m_world->SetChunkDirty(m_position + glm::ivec3(0, 0, 1));
		if (position.x == 0) m_world->SetChunkDirty(m_position + glm::ivec3(-1, 0, 0));
		if (position.y == 0) m_world->SetChunkDirty(m_position + glm::ivec3(0, -1, 0));
		if (position.z == 0) m_world->SetChunkDirty(m_position + glm::ivec3(0, 0, -1));
	}
	m_blocks[ToLocalBlockIndex(position)] = block;
	m_dirty = true;
}

uint8 Chunk::GetBlockFast(const glm::ivec3& position) const {
	return m_blocks[ToLocalBlockIndex(position)];
}

void Chunk::Compress(PacketWriter& writer) {
	//vector<pair<uint8, uint16>> tempVec;
	//uint8 currentBlock = m_blocks[0];
	//uint16 blockCount = 1;
	//for (uint32 i = 1; i < CHUNK_VOLUME; i++) {
	//	if (m_blocks[i] == currentBlock) {
	//		blockCount++;
	//	} else {
	//		tempVec.emplace_back(currentBlock, blockCount);
	//		currentBlock = m_blocks[i];
	//		blockCount = 1;
	//	}
	//}
	//tempVec.emplace_back(currentBlock, blockCount);
	//LOG("%d", tempVec.size());
	//writer.Write<uint32>(tempVec.size());
	//
	
	
	String compressed_data = gzip::compress((char*)m_blocks, CHUNK_VOLUME);
	const char* compressed_pointer = compressed_data.data();
	writer.Write<uint32>(compressed_data.size());
	writer.Write<const char>(compressed_pointer, compressed_data.size());
}

void Chunk::Decompress(PacketReader& reader) {
	uint32 size = reader.Read<uint32>();
	const char* data = reader.Read<const char>(size);
	String decompressed_data = gzip::decompress(data, size);
	memcpy((char*)m_blocks, decompressed_data.data(), CHUNK_VOLUME);
	//uint32 block = 0;
	//for (uint32 i = 0; i < size; i++) {
	//	auto& pair = pairs[i];
	//	for (int j = 0; j < pair.second; j++) {
	//		m_blocks[block++] = pair.first;
	//	}
	//}

	m_dirty = true;
}