#pragma once

class GameState : public State {
private:
	String m_name = "Game";
	AssetRef<Shader> m_geometryShader;
	AssetRef<Shader> m_uiShader;
	GroundRaycast m_rayCast;
	Vector2I m_rayCastPos;
	vector<Pointlight> m_pointlights;
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	Ludo* m_ludo;
	Ludo* m_ludo2;
	Model* model;
	Entity* entity;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		texNoise = GetAssetManager()->Get<Texture>("Noise");
		m_geometryShader = GetShaderManager()->Get("Geometry");
		m_uiShader = GetShaderManager()->Get("UI");
		m_pointlights.push_back(Pointlight(GetCamera()->m_position, 25, Color::White()));
		m_ludo = new Ludo();
		model = new Model();
		//model->LoadModel("res/sponza/sponza.obj");
		entity = new Entity(model);
		//entity->m_scale = Vector3(0.02f);
		map<char, TileDefinition*> m_table = {
			{'*', &TileDefinition::ANY },
			{'s', &TileDefinition::SOLID },
			{'e', &TileDefinition::EMPTY},
			{'r', &TileDefinition::EMPTY},
			{'b', &TileDefinition::SOLID2},
			{'h', &TileDefinition::HOOK}
		};
		m_ludo->SetTable(m_table);
		m_ludo->AddGrammar(0, Grammar(3, 3, "shsssssss", { "sessesshs", "seshehshs", "seshessss", "seshesshs", "seshehsss" }));
		m_ludo->AddGrammar(0, Grammar(6, 6, "**h****sss**sssss*sssss*sssss**sss**", { "**e*****e***srrrs*hrrrs*srrrs**shs**", "**e*****e***srrrs*hrrrh*srrrs**sss**", "**e*****e***srrrs*hrrrh*srrrs**shs**" }));
		m_ludo->AddGrammar(0, Grammar(9, 9, "****h*****sssssss**sssssss*sssssssssssssssssssssssssss*sssssss**sssssss****sss***", { "****e*****sssesss**seeeees**seeeees*sseeeeessheeeeeeehsseeeeess*sssesss****shs***",
		"****e*****sssesss**seeeees**seeeees*sseeeeessheeeeeeehsseeeeess*sssssss**********", "****e*****sssesss**seeeees**seeeees**seeeees**seeeees**seeeees**sssesss****shs***", "****e*****sssesss**seeeees**seeeees*sseeeeessheeeeeesssseeeeess*sssesss****shs***" }));
		m_ludo->AddGrammar(0, Grammar(5, 5, "*shs**sss**sss**sss**sss*", { "*ses**ses**ses**ses**shs*" }));
		m_ludo->AddGrammar(1, Grammar(1, 1, "h", { "s" }));
		m_ludo->AddGrammar(2, Grammar(3, 3, "****e**b*", { "****b**b*" }));
		m_ludo->AddCustomCode(3, [](Tile* tile) {
			for (int i = 0; i < tile->m_tiles.size(); i++) {
				if (tile->m_tiles[i]->IsSolid()) {
					Utils::RemoveFromVector(tile->m_tiles, tile->m_tiles[i]);
					Sleep(5);
				}
			}
		});
		GetWorld()->GetTile(14, 0)->SetDefinition(&TileDefinition::HOOK);
	}

	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);

		if (KeyJustDown('O')) {
			for (int x = -20; x < 20; x++) {
				for (int y = -20; y < 20; y++) {
					for (int z = -2; z < 20; z++) {
						if (Math::RandomInt(0, 10) > 8) m_pointlights.push_back(Pointlight(Vector3((float)x, (float)y, (float)z), 5, Color::RandomPrimary()));
					}
				}
			}
			//loop(x, GetWorld()->GetBoundaries().m_size.x) {
			//	loop(y, GetWorld()->GetBoundaries().m_size.y) {
			//		if (Math::RandomInt(0, 10) > 8) m_pointlights.push_back(Pointlight(Vector3((float)x, 1.2f, (float)y), 2, Color::RandomPrimary()));
			//	}
			//}
		}
		Vector3 cast = m_rayCast.GetGroundPosition(GetCamera());
		m_rayCastPos = m_rayCast.GetTile();

		float x = ((float)m_rayCastPos.x + 0.5f);
		float y = ((float)m_rayCastPos.y + 0.5f);

		m_pointlights[0].m_position.x = cast.x;
		m_pointlights[0].m_position.y = 1.2f;
		m_pointlights[0].m_position.z = cast.z;

		if (GetMouse()->ButtonDown(VK_MOUSE_LEFT)) {
			GetWorld()->BreakTile((int)x, (int)y);
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
		GetTileRenderer()->End();
		GetTileRenderer()->Draw();
		GetLineRenderer()->DrawRect(Rect((float)m_rayCastPos.x + 0.5f, (float)m_rayCastPos.y + 0.5f, 1.0f, 1.0f), Color::Red());

		Tile* tile = GetWorld()->GetTile(14, 0);
		DrawFromTileToTile(tile);


		GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries(), Color::Red());
		m_ludo->DebugDraw();
		loop(i, m_pointlights.size()) {
			GetPointlightRenderer()->Submit(m_pointlights[i]);
		}


		//m_geometryShader->Bind();
		//m_geometryShader->Set("_Iridescence", 4);
		//texIri->Bind(4);
		//m_geometryShader->Set("_IridescenceStrength", 0);
		//m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		//m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		//m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
		//m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);
		//m_geometryShader->Set("_TessellationFactor", (float)0);
		//m_geometryShader->Set("_TessellationSlope", (float)0);
		//m_geometryShader->Set("_TessellationShift", (float)0);
		//m_geometryShader->Set("_TessellationAlpha", (float)0);
		//entity->Draw(m_geometryShader, GL_PATCHES);
	}

	void DrawFromTileToTile(Tile* tile) {
		for (Tile* t : tile->m_tiles) {
			GetLineRenderer()->Submit(tile->m_position.x + 0.5f, 1.05f, tile->m_position.y + 0.5f, t->m_position.x + 0.5f, 1.05f, t->m_position.y + 0.5f, Color::Green());
			DrawFromTileToTile(t);
		}
	}

	void RenderUI() override {
		GetUIRenderer()->Rect(Origin::CENTER, Vector2(0.9f, 0.9f), Vector2(0.01f, 0.05f), Color::White(), GetCamera()->m_rotation.yaw);
	}

	void OnImGUI() override {
		ImGui::SliderFloat("scale1", &GetTileRenderer()->m_scale1, -5, 5);
		ImGui::SliderFloat("scale2", &GetTileRenderer()->m_scale2, -5, 5);
		ImGui::SliderFloat("roughness", &GetPipeline()->roughness, 0, 1);
		ImGui::SliderFloat("metallic", &GetPipeline()->metallic, 0, 1);
		ImGui::SliderFloat("_IridescenceStrength", &GetTileRenderer()->m_scale3, 0, 1);
		//ImGui::SliderFloat("Normal", &GetTileRenderer()->m_material->m_normalStrength, 0, 10);
		if (ImGui::Button("Iterate")) { m_ludo->Iterate(); }
		if (ImGui::Button("Execute")) { m_ludo->Execute(500); }
		if (ImGui::Button("Reset world")) {
			GetWorld()->Reset();
			GetWorld()->GetTile(14, 0)->SetDefinition(&TileDefinition::HOOK);
		}

		if (ImGui::Button("a")) GetWorld()->GetTile(14, 0)->SetDefinition(&TileDefinition::SOLID2);

		if (ImGui::Button("Generate Lock")) {
			Tile* tile = GetWorld()->GetTile(14, 0);
			tile->SetDefinition(&TileDefinition::LOCK);
		}
	}
	void Cleanup() override;
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};