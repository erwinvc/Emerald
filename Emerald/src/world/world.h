#pragma once
class TileRenderer;

class World : public Singleton<World> {
private:
	vector<Chunk> m_chunks;
	Rect m_boundaries;

	World() { Populate(); }
	~World() {}
	friend Singleton;

	void RecalculateBoundaries() {
		int xMin = 0, yMin = 0;
		int xMax = 0, yMax = 0;
		for (Chunk& chunk : m_chunks) {
			xMin = Math::Min(xMin, chunk.GetPosition().x);
			xMax = Math::Max(xMax, chunk.GetPosition().x);
			yMin = Math::Min(yMin, chunk.GetPosition().y);
			yMax = Math::Max(yMax, chunk.GetPosition().y);
		}
		xMax += 1;
		yMax += 1;
		xMin *= 16;
		xMax *= 16;
		yMin *= 16;
		yMax *= 16;

		m_boundaries = Rect((xMin + xMax) / 2, (yMin + yMax) / 2, Math::Abs(xMin) + Math::Abs(xMax), Math::Abs(yMin) + Math::Abs(yMax));

	}
public:
	const Rect& GetBoundaries() {
		return m_boundaries;
	}

	void Populate() {
		loop(y, 4) {
			loop(x, 4) {
				GenerateChunk(x, y);
			}
		}
		RecalculateBoundaries();
	}

	void GenerateChunk(int x, int y) {
		m_chunks.push_back(Chunk(x, y));
	}

	void Draw();

	Tile* GetTile(int x, int y) {
		int chunkX = x / 16;
		int chunkY = y / 16;
		int posX = x % 16;
		int posY = y % 16;

		for (int i = 0; i < m_chunks.size(); i++) {
			Vector2I pos = m_chunks[i].GetPosition();
			if (pos.x == chunkX && pos.y == chunkY) {
				return m_chunks[i].GetTile(posX, posY);
			}
		}
		return nullptr;
	}

	Tile* GetTile(const Vector2I& pos) {
		return GetTile(pos.x, pos.y);
	}

	void BreakTile(int x, int y) {
		Tile* tile = GetTile(x, y);
		if (tile) {
			tile->SetEmpty();
			//tile->m_heights = Vector4(0, 0);
			
			//WangTile::UpdateArea(x, y);
		}
	}

};

static World* GetWorld() { return World::GetInstance(); }