#include "stdafx.h"

bool Ludo::CheckGrammar(int x, int y, const Grammar& grammar, int grammarRotation) {
	int random = Math::RandomInt(0, grammar.m_toReplace.size() - 1);
	lastBounds = Rect(x + ((float)grammar.m_x / 2), y + ((float)grammar.m_y / 2), (float)grammar.m_x, (float)grammar.m_y);
	Tile* t;
	if (x + grammar.m_x > MAX_X) return false;
	if (y + grammar.m_y > MAX_Y) return false;
	for (int yy = 0; yy < grammar.m_y; yy++) {
		for (int xx = 0; xx < grammar.m_x; xx++) {
			lastBounds2 = Rect(x + xx + 0.5f, y + yy + 0.5f, 1.0f, 1.0f);
			//Sleep(100);
			Tile* tile = GetWorld()->GetTile(x + xx, y + yy);
			if (!tile) return false;
			int index = (yy * grammar.m_y) + xx;
			TileDefinition* toCheck = m_table[grammar.m_toFind[grammarRotation][index]];
			//LOG("%s, %s", tile->m_definition->GetName().c_str(), toCheck->GetName().c_str());
			if (toCheck == &TileDefinition::HOOK) {
				t = tile;
			}
			if (toCheck == &TileDefinition::ANY || tile->m_definition == toCheck) {
				color = Color::Green();
			} else {
				color = Color::Red();
				return false;
			}
		}
	}

	Sleep(10);
	for (int yy = 0; yy < grammar.m_y; yy++) {
		for (int xx = 0; xx < grammar.m_x; xx++) {
			Tile* tile = GetWorld()->GetTile(x + xx, y + yy);
			int index = (yy * grammar.m_y) + xx;
			TileDefinition* newDefinition = m_table[grammar.m_toReplace[random][grammarRotation][index]];
			if (newDefinition != &TileDefinition::ANY) {
				tile->SetDefinition(newDefinition);
				if (newDefinition == &TileDefinition::HOOK)
				{
					t->m_tiles.push_back(tile);
				}
				if (newDefinition == &TileDefinition::EMPTY) {
					GetWorld()->BreakTile(x + xx, y + yy);
				}
			}
		}
	}
	return true;
}