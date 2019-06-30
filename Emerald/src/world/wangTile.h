#pragma once

//class WangTile {
//public:
//	const Vector2I m_positionOffsets[] = {
//		{0, -1},
//		{},
//		{},
//		{},
//		{},
//		{},
//		{},
//		{}
//	};
//	static void UpdateArea(int x, int z)
//	{
//		Tile* right = GetWorld()->GetTile(x + 1, z);
//		if (right && right->IsSolid()) UpdateTile(right, true);
//		Tile* left = GetWorld()->GetTile(tile.m_position.x - 1, tile.m_position.y);
//		if (left && left->IsSolid()) UpdateTile(left, false);
//		Tile* down = GetWorld()->GetTile(tile.m_position.x, tile.m_position.y + 1);
//		if (down && down->IsSolid()) UpdateTile(down, false);
//		Tile* up = GetWorld()->GetTile(tile.m_position.x, tile.m_position.y - 1);
//		if (up && up->IsSolid()) UpdateTile(up, false);
//
//		Tile* upRight = GetWorld()->GetTile(tile.m_position.x + 1, tile.m_position.y - 1);
//		if (upRight && upRight->IsSolid()) UpdateTile(upRight, false);
//
//		Tile* upLeft = GetWorld()->GetTile(tile.m_position.x - 1, tile.m_position.y - 1);
//		if (upLeft && upLeft->IsSolid()) UpdateTile(upLeft, false);
//
//		Tile* downRight = GetWorld()->GetTile(tile.m_position.x + 1, tile.m_position.y + 1);
//		if (downRight && downRight->IsSolid()) UpdateTile(downRight, false);
//
//		Tile* downLeft = GetWorld()->GetTile(tile.m_position.x - 1, tile.m_position.y + 1);
//		if (downLeft && downLeft->IsSolid()) UpdateTile(downLeft, true);
//	}
//
//	static void UpdateTile(Tile& tile)
//	{
//		
//	}
//};