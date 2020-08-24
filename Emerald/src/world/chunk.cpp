#include "stdafx.h"
#include "gzip/compress.hpp"
#include "gzip/decompress.hpp"

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
	m_blocks[ToLocalBlockIndex(position)] = BlockState(block);
	m_dirty = true;
}

bool Chunk::GetBlock(const glm::ivec3& position, BlockState*& blockState) {
	if (BlockPositionOutOfChunkBounds(position)) {
		if (GetApp()->IsServer()) {
			ServerWorld* m_world = GetServer()->GetWorld();
			return m_world->GetBlock(ToGlobalBlockPosition(position, m_position), blockState);
		} else {
			ClientWorld* m_world = GameStates::VOXEL->GetWorld();
			return m_world->GetBlock(ToGlobalBlockPosition(position, m_position), blockState);
		}
	}
	blockState = &m_blocks[ToLocalBlockIndex(position)];
	return true;
}

bool Chunk::GetBlockFast(const glm::ivec3& position, BlockState*& blockState) {
	blockState = &m_blocks[ToLocalBlockIndex(position)];
	return true;
}

bool Chunk::GetBlockAtIndex(uint32 index, BlockState*& blockState) {
	ASSERT(index >= 0 && index < CHUNK_VOLUME, "Out of chunk bounds");
	blockState = &m_blocks[index];
	return true;
}

uint8 Chunk::GetBlockID(const glm::ivec3& position) const {
	if (BlockPositionOutOfChunkBounds(position)) {
		if (GetApp()->IsServer()) {
			ServerWorld* m_world = GetServer()->GetWorld();
			return m_world->GetBlockID(ToGlobalBlockPosition(position, m_position));
		} else {
			ClientWorld* m_world = GameStates::VOXEL->GetWorld();
			return m_world->GetBlockID(ToGlobalBlockPosition(position, m_position));
		}
	}
	return m_blocks[ToLocalBlockIndex(position)].blockID;
}

void Chunk::BreakBlock(const BlockPos& blockPosition, BlockSide blockSide) {
	BlockState& blockState = m_blocks[ToLocalBlockIndex(blockPosition)];
	switch (blockSide) {
		case BlockSide::WEST: blockState.dWest++; break;
		case BlockSide::EAST: blockState.dEast++; break;
		case BlockSide::SOUTH: blockState.dSouth++; break;
		case BlockSide::NORTH: blockState.dNorth++; break;
		case BlockSide::UP: blockState.dUp++; break;
		case BlockSide::DOWN: blockState.dDown++; break;
	}

	if (blockState.dWest + blockState.dEast == 16) m_blocks[ToLocalBlockIndex(blockPosition)] = BlockState(0);
	if (blockState.dSouth + blockState.dNorth == 16) m_blocks[ToLocalBlockIndex(blockPosition)] = BlockState(0);
	if (blockState.dUp + blockState.dDown == 16) m_blocks[ToLocalBlockIndex(blockPosition)] = BlockState(0);
}

void Chunk::Draw() {
	if (m_dirty) GenerateMesh();
	m_mesh.m_mesh->Draw();
}

void Chunk::GenerateMesh() {
	m_mesh = ChunkMeshGenerator::MakeChunkMesh(*this);
	m_mesh.GenerateMesh();
	m_dirty = false;
}

void Chunk::Compress(PacketWriter& writer) {
	String compressed_data = gzip::compress((char*)m_blocks, CHUNK_VOLUME * sizeof(BlockState));
	const char* compressed_pointer = compressed_data.data();
	writer.Write<uint32>(compressed_data.size());
	writer.Write<const char>(compressed_pointer, compressed_data.size());
}

void Chunk::Decompress(PacketReader& reader) {
	uint32 size = reader.Read<uint32>();
	const char* data = reader.Read<const char>(size);
	String decompressed_data = gzip::decompress(data, size);
	memcpy((char*)m_blocks, decompressed_data.data(), CHUNK_VOLUME * sizeof(BlockState));
	m_dirty = true;
}