#pragma once

class World {
public:
	TileGrid* m_grid;

	World() {
		m_grid = NEW(TileGrid());
	}
	~World() {
		DELETE(m_grid);
	}

	void Update(const TimeStep& time) {

	}

	void RenderGeometry() {
		m_grid->RenderGeometry();
	}

	friend void to_json(nlohmann::json& j, const World& w) {
		j = nlohmann::json({ "grid", *w.m_grid });
	}
};