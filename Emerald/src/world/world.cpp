#include "stdafx.h"
#include "world.h"

void World::Draw(ManagedRef<TileRenderer> renderer) {
	for(Chunk& chunk : m_chunks) {
		chunk.Draw(renderer);
	}
}