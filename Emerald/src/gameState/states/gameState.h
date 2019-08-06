#pragma once

class GameState : public State {
private:
	String m_name = "Game";
	AssetRef<Shader> m_geometryShader;
	AssetRef<Shader> m_uiShader;

	ManagedRef<TileRenderer> m_tileRenderer;

	GroundRaycast m_rayCast;

public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_geometryShader = GetShaderManager()->Get("GEO");
		m_uiShader = GetShaderManager()->Get("UI");

		m_tileRenderer = NEW(TileRenderer());

	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
		if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
			GetPipeline()->GetPointLights().push_back(Pointlight(GetCamera()->m_position, 10, Color::RandomPrimary()));
		}
	}
	void RenderGeometry() override {
		m_tileRenderer->Begin();
		GetPipeline()->GetGBuffer()->BindTextures();

		GetWorld()->Draw(m_tileRenderer);
		m_tileRenderer->End();
		m_tileRenderer->Draw();

		Vector3 cast = m_rayCast.Get(GetCamera());
		Vector2I pos = m_rayCast.GetTile();
		GetLineRenderer()->DrawRect(Rect((float)pos.x + 0.5f, (float)pos.y + 0.5f, 1.0f, 1.0f));

		//if (GetMouse()->ButtonJustDown(VK_MOUSE_LEFT)) {
		//	Tile* t = GetWorld()->GetTile(((int)pos.x + 0.5f), ((int)pos.z + 0.5f));
		//	if (t)
		//		t->m_type = EMPTY;
		//}
		GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries());
	}
	void RenderUI() override {
	}
	void OnImGUI() override
	{
		ImGui::SliderFloat("scale1", &m_tileRenderer->m_scale1, 0, 5);
		ImGui::SliderFloat("scale2", &m_tileRenderer->m_scale2, 0, 5);
		ImGui::SliderFloat("scale3", &m_tileRenderer->m_scale3, 0, 1);
		ImGui::SliderFloat("Normal", &m_tileRenderer->m_material->m_normalStrength, 0, 10);

	}
	void Cleanup() override;

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};