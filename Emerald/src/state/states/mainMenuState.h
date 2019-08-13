#pragma once

struct TileCombo {
	TileType m_type;
	TileTransform m_transform;
	bool m_set;

	TileCombo(TileType type, TileTransform transform) : m_type(type), m_transform(transform), m_set(true) {}

	TileCombo() : m_type(GROUND), m_transform(UP), m_set(false) {}

	String GetAsString() {
		if (!m_set) return "Not set";
		if (m_type == GROUND) {
			return "GROUND";
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
		m_selectedTile->Set(GROUND, UP);
		for (int i = 0; i < 8; i++) {
			m_tiles[i] = new Tile();
			m_tilesBools[i] = false;
		}
	}

	void Update(const TimeStep& time) override {
		if (m_enableCamera)GetCamera()->Update(time);
		if (GetKeyboard()->KeyJustDown('1')) {
			int newTile = m_selectedTile->m_type + 1;
			if (newTile > TileRenderer::TILECOUNT) newTile = 0;
			m_selectedTile->Set(TileType(newTile), UP);
		} else if (GetKeyboard()->KeyJustDown('R')) {
			if (m_selectedTile->m_type != 0 && m_selectedTile->m_type != -1)
				m_selectedTile->SetTransform(RotateTileTransform(m_selectedTile->m_transformIndex, 1));
		} else if (GetKeyboard()->KeyJustDown('2')) {
			int newTile = m_selectedTile->m_type - 1;
			if (newTile < 0) newTile = TileRenderer::TILECOUNT - 1;
			m_selectedTile->Set(TileType(newTile), UP);
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
				m_combos[(wangIndex * 4) % 255] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 1));
				m_combos[(wangIndex * 4 * 4) % 255] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 2));
				m_combos[(wangIndex * 4 * 4 * 4) % 255] = TileCombo(m_selectedTile->m_type, RotateTileTransform(m_selectedTile->m_transformIndex, 3));
			}
		}

		if (ImGui::Button("Export")) {
			nlohmann::json jsonOb;
			for (int i = 0; i < 256; i++) {
				jsonOb[Format_t("%d", i)] = { {"type", m_combos[i].m_type}, {"transform", m_combos[i].m_transform}, {"set", m_combos[i].m_set} };
			}
			FileSystem::SaveJsonToFile(jsonOb, "wangTiles");
		}

		if (ImGui::Button("Translate")) {
			nlohmann::json jsonOb = FileSystem::LoadJsonFromFile("wangTiles");
			try {
				map<int, map<int, vector<int>>> m_values;
				for (int i = 0; i < 256; i++) {
					String iAsString = to_string(i);
					if (m_values.find(jsonOb[iAsString]["type"]) == m_values.end()) {
						auto a = map<int, vector<int>>();
						m_values[jsonOb[iAsString]["type"]] = a;
					}
					map<int, vector<int>>& m_value = m_values[jsonOb[iAsString]["type"]];

					if (m_value.find(jsonOb[iAsString]["transform"]) == m_value.end()) {
						auto a = vector<int>();
						m_value[jsonOb[iAsString]["transform"]] = a;
					}
					m_value[jsonOb[iAsString]["transform"]].push_back(i);
				}

				nlohmann::json jsonOb2;
				for (auto tile = m_values.begin(); tile != m_values.end(); tile++) {
					for (auto it2 = tile->second.begin(); it2 != tile->second.end(); it2++)
						jsonOb2[to_string(tile->first)][to_string(it2->first)] = it2->second;
				}
				FileSystem::SaveJsonToFile(jsonOb2, "wangTilesTranslated");

				String finalStr;
				for (auto tile = m_values.begin(); tile != m_values.end(); tile++) {
					for (auto transform = tile->second.begin(); transform != tile->second.end(); transform++) {
						for (int i = 0; i < transform->second.size(); i++) {
							if (i == transform->second.size()) finalStr += Format("case %d: tile->Set(%s, %s); break;", i, TileTypeToString(TileType(tile->first)).c_str(), TileTransformToString(TileTransform(transform->first)).c_str());
							else finalStr += Format("case %d:", i);
						}
					}
				}

				FileSystem::SaveStringToFile(finalStr, "wangTilesSwitch");

			} catch (nlohmann::json::parse_error& e) {
				LOG("Failed to load translation file: %d", e.id);
			}
		}
		ImGui::Checkbox("Camera enabled", &m_enableCamera);

		ImGui::LabelText("CurrentTile: ", TileCombo(m_selectedTile->m_type, m_selectedTile->m_transformIndex).GetAsString().c_str());

		ImGui::InputText("Output", buffer, sizeof(buffer));

		if (ImGui::SliderInt("Index", &wangIndex, 0, 255)) {
			m_selectedTile->Set(m_combos[wangIndex].m_type, m_combos[wangIndex].m_transform);
		}
		ImGui::LabelText("Index ", Format_t("%d", wangIndex));

		ImGui::Separator();

		ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth(), 500), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < 256; i++) {
			if (ImGui::Selectable(Format_t("%d %s", i, m_combos[i].GetAsString().c_str()), i == wangIndex)) {
				wangIndex = i;
				m_selectedTile->Set(m_combos[wangIndex].m_type, m_combos[wangIndex].m_transform);
			}
		}
		ImGui::EndChild();

	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};