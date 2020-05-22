#include "stdafx.h"

uint8 Chunk::GetBlock(const glm::ivec3& position) const {
	if (BlockPositionOutOfChunkBounds(position)) {
		return m_world->GetBlock(ToGlobalBlockPosition(position, m_position));
	}
	return m_blocks[ToLocalBlockIndex(position)];
}


void Chunk::SetBlock(const glm::ivec3& position, uint8 block) {
	m_blocks[ToLocalBlockIndex(position)] = block;
	if (position.x == 31) m_world->SetChunkDirty(m_position + glm::ivec3(1, 0, 0));
	if (position.y == 31) m_world->SetChunkDirty(m_position + glm::ivec3(0, 1, 0));
	if (position.z == 31) m_world->SetChunkDirty(m_position + glm::ivec3(0, 0, 1));
	if (position.x == 0) m_world->SetChunkDirty(m_position + glm::ivec3(-1, 0, 0));
	if (position.y == 0) m_world->SetChunkDirty(m_position + glm::ivec3(0, -1, 0));
	if (position.z == 0) m_world->SetChunkDirty(m_position + glm::ivec3(0, 0, -1));
	m_dirty = true;
}

uint8 Chunk::GetBlockFast(const glm::ivec3& position) const {
	return m_blocks[ToLocalBlockIndex(position)];
}