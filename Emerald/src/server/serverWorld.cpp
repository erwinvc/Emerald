#include "stdafx.h"

ServerWorld::ServerWorld() {
	m_entities.resize(32);

	for (int z = -1; z < 1; z++) {
		for (int x = -1; x < 1; x++) {
			for (int y = -16; y < 0; y++) {
				m_chunks.emplace(glm::ivec3(x, y, z), Chunk(glm::ivec3(x, y, z), true));
			}
		}
	}

	for (int z = -1; z < 1; z++) {
		for (int x = -1; x < 1; x++) {
			m_chunks.emplace(glm::ivec3(x, 0, z), Chunk(glm::ivec3(x, 0, z), false));
		}
	}

	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
		auto& chunk = *it;
		for (int y = 0; y < 32; y++) {
			for (int z = 0; z < 32; z++) {
				for (int x = 0; x < 32; x++) {
					int yy = y + (32 * chunk.first.y);
					if (chunk.first.y >= 0) {
						chunk.second.SetBlock(glm::ivec3(x, y, z), 0);
					} else if (yy < -6) {
						chunk.second.SetBlock(glm::ivec3(x, y, z), Random::Int(0, 2) == 0 ? Random::Int(1, 2) : 1);
					} else chunk.second.SetBlock(glm::ivec3(x, y, z), 1);
				}
			}
		}
	}
}

uint32 ServerWorld::AddEntity() {
	for (int i = 1; i < m_entities.size(); i++) {
		auto& state = m_entities[i];
		if (!state.active) {
			m_entityCount++;
			state.active = true;
			return i;
		}
	}
	return 0;
}



void ServerWorld::Update() {
	m_time++;
}