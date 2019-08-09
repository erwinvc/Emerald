#pragma once

struct TileCombo {
	TileType m_type;
	TileTransform m_transform;
	bool m_set;

	TileCombo(TileType type, TileTransform transform) : m_type(type), m_transform(transform), m_set(true) {}

	TileCombo() : m_type(EMPTY), m_transform(LOW), m_set(false) {}

	String GetAsString() {
		if (!m_set) return "Not set";
		if (m_type == EMPTY) {
			return "EMPTY";
		} else if (m_type == FULL) {
			return "FULL";
		}
		return Format_t("%s : %s", TileTypeToString(m_type).c_str(), TileTransformToString(m_transform).c_str());
	}

};
class MainMenuState : public State {
private:
	String m_name = "MainMenu";
	Tile* m_selectedTile;
	TileTransform transformIndex;

	Tile* m_tiles[8];
	bool m_tilesBools[8];
	int wangIndex = 0;

	bool m_enableCamera = true;

	TileCombo m_combos[256] = { TileCombo() };
	char buffer[0xffff];

public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_selectedTile = new Tile();

		for (int i = 0; i < 8; i++) {
			m_tiles[i] = new Tile();
			m_tilesBools[i] = false;
		}
	}

	void Update(const TimeStep& time) override {
		if (m_enableCamera)GetCamera()->Update(time);
		if (GetKeyboard()->KeyJustDown('1')) {
			m_selectedTile->SetEmpty();
		} else if (GetKeyboard()->KeyJustDown('R')) {
			if (m_selectedTile->m_type != 0 && m_selectedTile->m_type != -1)
				m_selectedTile->SetTransform(RotateTileTransform(m_selectedTile->m_transformIndex, 1));
		} else if (GetKeyboard()->KeyJustDown('2')) {
			m_selectedTile->SetFull();
		} else if (GetKeyboard()->KeyJustDown('3')) {
			m_selectedTile->SetType(INNER);
		} else if (GetKeyboard()->KeyJustDown('4')) {
			m_selectedTile->SetType(OUTER);
		} else if (GetKeyboard()->KeyJustDown('5')) {
			m_selectedTile->SetType(SLOPE);
		} else if (GetKeyboard()->KeyJustDown('6')) {
			m_selectedTile->SetType(VALLEY);
		}

		if (GetKeyboard()->KeyJustDown('N')) {
			wangIndex++;
		}

		m_tilesBools[0] = (wangIndex & 1) == 1;
		m_tilesBools[1] = (wangIndex & 2) == 2;
		m_tilesBools[2] = (wangIndex & 4) == 4;
		m_tilesBools[3] = (wangIndex & 8) == 8;
		m_tilesBools[4] = (wangIndex & 16) == 16;
		m_tilesBools[5] = (wangIndex & 32) == 32;
		m_tilesBools[6] = (wangIndex & 64) == 64;
		m_tilesBools[7] = (wangIndex & 128) == 128;
	}

	const Vector2I m_positionOffsets[8] = {
		{0, 1},  //North
		{-1, 1}, //NorthEase
		{-1, 0}, //East
		{-1, -1},//SouthEast
		{0, -1}, //South
		{1, -1}, //SouthWest
		{1, 0},	 //West
		{1, 1}	 //Northwest
	};

	void RenderTile(int index, int x, int y) {
		if (m_tilesBools[index]) {
			Tile* tile = m_tiles[index];
			GetTileRenderer()->Submit(*tile, x, y);
		}
	}
	void RenderGeometry() override {
		GetPipeline()->GetGBuffer()->BindTextures();

		GetTileRenderer()->Begin();

		GetTileRenderer()->Submit(*m_selectedTile, 0, 0);

		for (int i = 0; i < 8; i++) {
			Vector2I pos = m_positionOffsets[i];
			Rect r((float)pos.x + 0.5f, (float)pos.y + 0.5f, 1.0f, 1.0f);
			GetLineRenderer()->DrawRect(r);
			RenderTile(i, pos.x, pos.y);
		}
		GetTileRenderer()->End();
		GetTileRenderer()->Draw();
	}
	void RenderUI() override {}
	void OnImGUI() override {
		if (ImGui::Button("Next")) {
			wangIndex++;
			if (wangIndex > 255) wangIndex = 0;
			m_selectedTile->Set(m_combos[wangIndex].m_type, m_combos[wangIndex].m_transform);
		}

		if (ImGui::Button("Previous")) {
			wangIndex--;
			if (wangIndex < 0) wangIndex = 255;
			m_selectedTile->Set(m_combos[wangIndex].m_type, m_combos[wangIndex].m_transform);
		}

		if (m_combos[wangIndex].m_set ? ImGui::Button("Overwrite") : ImGui::Button("Save")) {
			m_combos[wangIndex] = TileCombo(m_selectedTile->m_type, m_selectedTile->m_transformIndex);
			if (wangIndex != 0) {
				m_combos[wangIndex * 4] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 1));
				m_combos[wangIndex * 4 * 4] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 2));
				m_combos[wangIndex * 4 * 4 * 4] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 3));
			}
		}

		if (ImGui::Button("Export")) {
			nlohmann::json jsonOb;
			for (int i = 0; i < 256; i++) {
				jsonOb[Format_t("%d", i)] = { {"type", m_combos[i].m_type}, {"transform", m_combos[i].m_transform}, {"set", m_combos[i].m_set} };
			}
			JsonUtils::SaveToFile(jsonOb, "wangTiles");
		}
		ImGui::Checkbox("Camera enabled", &m_enableCamera);

		ImGui::LabelText("CurrentTile: ", TileCombo(m_selectedTile->m_type, m_selectedTile->m_transformIndex).GetAsString().c_str());

		ImGui::InputText("Output", buffer, sizeof(buffer));

		if (ImGui::SliderInt("Index", &wangIndex, 0, 255)) {
			m_selectedTile->Set(m_combos[wangIndex].m_type, m_combos[wangIndex].m_transform);
		}
		ImGui::LabelText("Index ", Format_t("%d", wangIndex));

		ImGui::Separator();

		for (int i = 0; i < 256; i++) {
			ImGui::LabelText(Format_t("%d", i), m_combos[i].GetAsString().c_str());
		}
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};