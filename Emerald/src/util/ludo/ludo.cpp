#include "stdafx.h"

bool Ludo::CheckGrammar(int x, int y, Grammar& grammar, int grammarRotation, Tile*& tempTile) {
	lastBounds = Rect(x + ((float)grammar.m_x / 2), y + ((float)grammar.m_y / 2), (float)grammar.m_x, (float)grammar.m_y);
	if (x + grammar.m_x > MAX_X) return false;
	if (y + grammar.m_y > MAX_Y) return false;
	for (int yy = 0; yy < grammar.m_y; yy++) {
		for (int xx = 0; xx < grammar.m_x; xx++) {
			lastBounds2 = Rect(x + xx + 0.5f, y + yy + 0.5f, 1.0f, 1.0f);
			Tile* tile = GetWorld()->GetTile(x + xx, y + yy);
			if (!tile) return false;
			int index = (yy * grammar.m_y) + xx;
			TileDefinition* toCheck = m_table[grammar.m_toFind[grammarRotation][index]];
			if (toCheck == &TileDefinition::HOOK) {
				tempTile = tile;
			}
			if (toCheck == &TileDefinition::ANY || tile->m_definition == toCheck) {
				color = Color::Green();
			} else {
				color = Color::Red();
				return false;
			}
		}
	}
	return true;
}

void Ludo::ApplyGrammar(int x, int y, Grammar& grammar, int grammarRotation, Tile* tempTile) {
	int random = Math::RandomInt(0, grammar.m_toReplace.size() - 1);
	for (int yy = 0; yy < grammar.m_y; yy++) {
		for (int xx = 0; xx < grammar.m_x; xx++) {
			Tile* tile = GetWorld()->GetTile(x + xx, y + yy);
			int index = (yy * grammar.m_y) + xx;
			TileDefinition* newDefinition = m_table[grammar.m_toReplace[random][grammarRotation][index]];
			if (newDefinition != &TileDefinition::ANY) {
				tile->SetDefinition(newDefinition);
				if (newDefinition == &TileDefinition::HOOK) {
					tempTile->m_tiles.push_back(tile);
					tile->m_parent = tempTile;
				}
				if (newDefinition == &TileDefinition::EMPTY || newDefinition == &TileDefinition::ROOM) {
					GetWorld()->BreakTile(x + xx, y + yy);
				}
				if (newDefinition == &TileDefinition::LOCK) {
					tile->SetFull();
					//WangTile::UpdateTile(tile->m_position, tile);
				}
			}
		}
	}
}
