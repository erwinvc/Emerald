#pragma once

class World {
private:
	TileGrid* m_grid;

public:
	World() {
		m_grid = NEW(TileGrid());
	}
	~World() {
		DELETE(m_grid);
	}

	void Update(const TimeStep& time) {

	}

	void RenderGeometry() {
		m_grid->DrawGeometry();
	}
};