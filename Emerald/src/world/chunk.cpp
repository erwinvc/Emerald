#include "stdafx.h"

//#TODO culling
void Chunk::Draw(TileRenderer* renderer) {
	loop(y, 16) {
		loop(x, 16) {
			renderer->Submit(m_tiles[y][x], x + m_position.x * 16, y + m_position.y * 16);
		}
	}
}