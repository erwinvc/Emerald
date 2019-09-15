#pragma once

class Chunk {
private:
	Vector2I m_position;
	Tile m_tiles[16][16];
	bool m_populated = false;

public:
	void Populate() {
		if (m_populated) return;
		Reset();
		m_populated = true;
	}

	void Reset() {
		loop(y, 16) {
			loop(x, 16) {
				m_tiles[y][x] = Tile(&TileDefinition::SOLID, Vector2I(x, y) + m_position * 16);
			}
		}
	}

	Tile* GetTile(int x, int y) {
		if (Math::Within(x, 0, 16) && Math::Within(y, 0, 16))
			return &m_tiles[y][x];
		return nullptr;
	}

	const Vector2I& GetPosition() {
		return m_position;
	}

	void Draw();

	Chunk(int x, int y) : m_position(x, y) { Populate(); }
};

