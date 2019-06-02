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
		Logger::LogMessage(m_boundaries);
	}
public:
	const Rect& GetBoundaries() {
		return m_boundaries;
	}

	void Populate() {
		loop(y, 1) {
			loop(x, 1) {
				GenerateChunk(x, y);
			}
		}
		RecalculateBoundaries();
	}

	void GenerateChunk(int x, int y) {
		m_chunks.push_back(Chunk(x, y));
	}

	void Draw(TileRenderer* renderer);

};

static World* GetWorld() { return World::GetInstance(); }