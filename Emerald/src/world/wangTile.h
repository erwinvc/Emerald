#pragma once

class WangTile {
public:
	static const Vector2I m_positionOffsets[8];

	static void UpdateArea(int x, int z);
	static void UpdateTile(Vector2I& pos, Tile* tile);
};