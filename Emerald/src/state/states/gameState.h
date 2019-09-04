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
	AssetRef<Model> m_model;
	//Pointlight* pl;
	vector<Pointlight> m_pointlights;
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		texNoise = GetAssetManager()->Get<Texture>("Noise");
		m_model = GetAssetManager()->Get<Model>("dragon");
		m_geometryShader = GetShaderManager()->Get("Geometry");
		m_uiShader = GetShaderManager()->Get("UI");
		m_pointlights.push_back(Pointlight(GetCamera()->m_position, 25, Color::White()));
		//pl = &m_pointlights[0];
	}

	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);

		//if (KeyJustDown('O')) {
		//	loop(x, GetWorld()->GetBoundaries().m_size.x) {
		//		loop(y, GetWorld()->GetBoundaries().m_size.y) {
		//			if (Math::RandomInt(0, 10) > 8) m_pointlights.push_back(Pointlight(Vector3((float)x, 1.2f, (float)y), 2, Color::RandomPrimary()));
		//		}
		//	}
		//}
		Vector3 cast = m_rayCast.GetGroundPosition(GetCamera());
		m_rayCastPos = m_rayCast.GetTile();

		float x = ((float)m_rayCastPos.x + 0.5f);
		float y = ((float)m_rayCastPos.y + 0.5f);

		m_pointlights[0].m_position.x = cast.x;
		m_pointlights[0].m_position.y = 1.2f;
		m_pointlights[0].m_position.z = cast.z;

		if (GetMouse()->ButtonDown(VK_MOUSE_LEFT)) {
			GetWorld()->BreakTile(x, y);
		}

		if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
			m_pointlights.push_back(Pointlight(Vector3(cast.x, 1.5f, cast.z), 25, Color(1, 0.8f, 0.8f)));
		}
		if (KeyJustDown('H')) {
			m_pointlights.push_back(Pointlight(Vector3(cast.x, 1.5f, cast.z), 25, Color::RandomPrimary()));
		}
	}

	void RenderGeometry() override {
		GetPipeline()->GetGBuffer()->BindTextures();
		GetTileRenderer()->Begin();
		GetWorld()->Draw();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GetTileRenderer()->End();
		GetTileRenderer()->Draw();
		GetLineRenderer()->DrawRect(Rect((float)m_rayCastPos.x + 0.5f, (float)m_rayCastPos.y + 0.5f, 1.0f, 1.0f));

		loop(i, m_pointlights.size()) {
			GetPointlightRenderer()->Submit(m_pointlights[i]);
		}

		GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries());
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		m_geometryShader->Bind();
		m_geometryShader->Set("texture_iridescence", 4);
		texIri->Bind(4);
		m_geometryShader->Set("texture_noise", 5);
		texNoise->Bind(5);
		m_geometryShader->Set("scale1", 0);
		m_geometryShader->Set("scale2", 5);
		m_geometryShader->Set("scale3", 0);
		m_geometryShader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
		m_geometryShader->Set("viewMatrix", GetCamera()->GetViewMatrix());
		m_geometryShader->Set("transformationMatrix", Matrix4::Identity());
		m_model->Draw(m_geometryShader);
	}

	void RenderUI() override {
		GetUIRenderer()->Rect(Origin::CENTER, Vector2(0.9f, 0.9f), Vector2(0.01f, 0.05f), Color::White(), GetCamera()->m_rotation.yaw);
	}
	void OnImGUI() override {
		ImGui::SliderFloat("scale1", &GetTileRenderer()->m_scale1, -5, 5);
		ImGui::SliderFloat("scale2", &GetTileRenderer()->m_scale2, -5, 5);
		ImGui::SliderFloat("scale3", &GetTileRenderer()->m_scale3, 0, 1);
		//ImGui::SliderFloat("Normal", &GetTileRenderer()->m_material->m_normalStrength, 0, 10);
	}
	void Cleanup() override;
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};