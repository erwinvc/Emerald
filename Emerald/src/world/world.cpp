#include "stdafx.h"
#include "world.h"

void World::Draw() {
	for(Chunk& chunk : m_chunks) {
		chunk.Draw();
	}
}