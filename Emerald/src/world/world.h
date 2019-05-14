#pragma once
class TileRenderer;

class World {

	vector<Chunk> m_chunks;
public:
	void Populate() {
		loop(y, 16) {
			loop(x, 16) {
				GenerateChunk(x, y);
			}
		}
	}

	void GenerateChunk(int x, int y) {
		m_chunks.push_back(Chunk(x, y));
	}

	void Draw(TileRenderer* renderer);

	World() { Populate(); }
};

