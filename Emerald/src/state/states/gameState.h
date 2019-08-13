#pragma once

class Test {
public:
	int j = 0;
	int i = 0;
};

class GameState : public State {
private:
	String m_name = "Game";
	AssetRef<Shader> m_geometryShader;
	AssetRef<Shader> m_uiShader;

	GroundRaycast m_rayCast;
	Vector2I m_rayCastPos;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_geometryShader = GetShaderManager()->Get("GEO");
		m_uiShader = GetShaderManager()->Get("UI");

	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
		if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
			GetPipeline()->GetPointLights().push_back(Pointlight(GetCamera()->m_position, 10, Color::RandomPrimary()));
		}

		if (KeyJustDown('O')) {
			loop(x, GetWorld()->GetBoundaries().m_size.x) {
				loop(y, GetWorld()->GetBoundaries().m_size.y) {
					if (Math::RandomInt(0, 10) > 8) GetPipeline()->GetPointLights().push_back(Pointlight(Vector3((float)x, 1.2f, (float)y), 2, Color::RandomPrimary()));

				}
			}
		}
		Vector3 cast = m_rayCast.Get(GetCamera());
		m_rayCastPos = m_rayCast.GetTile();

		if (GetMouse()->ButtonDown(VK_MOUSE_LEFT)) {
			int x = (int)((float)m_rayCastPos.x + 0.5f);
			int y = (int)((float)m_rayCastPos.y + 0.5f);
			Timer timer;
			GetWorld()->BreakTile(x, y);
			LOG("%.2fms", timer.Get());
		}
	}

	void RenderGeometry() override {
		GetPipeline()->GetGBuffer()->BindTextures();
		GetTileRenderer()->Begin();
		GetWorld()->Draw();
		GetTileRenderer()->End();
		GetTileRenderer()->Draw();
		GetLineRenderer()->DrawRect(Rect((float)m_rayCastPos.x + 0.5f, (float)m_rayCastPos.y + 0.5f, 1.0f, 1.0f));

		GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries());
	}
	void RenderUI() override {
		GetUIRenderer()->Rect(Origin::CENTER, Vector2(0.9f, 0.9f), Vector2(0.01f, 0.05f), Color::White(), GetCamera()->m_rotation.yaw);
	}
	void OnImGUI() override {
		ImGui::SliderFloat("scale1", &GetTileRenderer()->m_scale1, 0, 5);
		ImGui::SliderFloat("scale2", &GetTileRenderer()->m_scale2, 0, 5);
		ImGui::SliderFloat("scale3", &GetTileRenderer()->m_scale3, 0, 1);
		ImGui::SliderFloat("Normal", &GetTileRenderer()->m_material->m_normalStrength, 0, 10);
	}
	void Cleanup() override;

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override
	{
	}
};