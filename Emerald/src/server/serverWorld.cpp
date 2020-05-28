#include "stdafx.h"

ServerWorld::ServerWorld() {
	m_entities.resize(32);

	for (int z = -1; z < 1; z++) {
		for (int x = -1; x < 1; x++) {
			for (int y = -16; y < 0; y++) {
				m_chunks.emplace(glm::ivec3(x, y, z), Chunk(glm::ivec3(x, y, z), this));
			}
		}
	}

	for (int z = -1; z < 1; z++) {
		for (int x = -1; x < 1; x++) {
			m_chunks.emplace(glm::ivec3(x, 0, z), Chunk(glm::ivec3(x, 0, z), this, false));
		}
	}
}

uint32 ServerWorld::AddEntity() {
	for (int i = 1; i < m_entities.size(); i++) {
		auto& state = m_entities[i];
		if (!state.active) {
			//entityCount++;
			state.active = true;
			return i;
		}
	}
	return 0;
}

