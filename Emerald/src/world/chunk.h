#pragma once

class Chunk {
private:
	Vector2I m_position;
	Tile m_tiles[16][16];
	bool m_populated = false;

public:
	void Populate() {
		if (m_populated) return;
		//siv::PerlinNoise noise;
		loop(y, 16) {
			loop(x, 16) {
				m_tiles[y][x] = Tile();
				//float dx1 = (((float)m_position.x * 16) + (float)x + 0.5f) / 8;
				//float dx2 = (((float)m_position.x * 16) + (float)x - 0.5f) / 8;
				//float dy1 = (((float)m_position.y * 16) + (float)y + 0.5f) / 8;
				//float dy2 = (((float)m_position.y * 16) + (float)y - 0.5f) / 8;
				//Vector4 heights(
				//	(float)noise.noise(dx1, dy1),
				//	(float)noise.noise(dx1, dy2),
				//	(float)noise.noise(dx2, dy2),
				//	(float)noise.noise(dx2, dy1)
				//);
				//m_tiles[y][x].m_heights = heights;
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

	void Draw();

	Chunk(int x, int y) : m_position(x, y) { Populate(); }
};

