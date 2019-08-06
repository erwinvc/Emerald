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
				m_tiles[y][x] = Tile(Math::RandomInt(0, 3));
			}
		}
		m_populated = true;
	}

	Tile* GetTile(int x, int y) {
		if (Math::Within(x, 0, 16) && Math::Within(y, 0, 16))
			return &m_tiles[y][x];
		return nullptr;
	}

	const Vector2I& GetPosition() {
		return m_position;
	}

	void Draw(ManagedRef<TileRenderer> renderer);

	Chunk(int x, int y) : m_position(x, y) { Populate(); }
};

