#pragma once

class Chunk {
private:
	Vector2I m_position;
	Tile m_tiles[16][16];
	bool m_populated = false;

public:
	void Populate() {
		if (m_populated) return;
		loop(y, 16) {
			loop(x, 16) {
				m_tiles[y][x] = Tile(Math::RandomInt(0, 4));
			}
		}
		m_populated = true;
	}

	const Vector2I& GetPosition() {
		return m_position;
	}

	void Draw(TileRenderer* renderer);

	Chunk(int x, int y) : m_position(x, y) { Populate(); }
};

