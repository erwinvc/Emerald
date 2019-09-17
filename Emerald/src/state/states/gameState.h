#pragma once
struct Collectible {
	Entity* m_entity;
	Pointlight m_pointLight;
	bool m_removed = false;

	inline bool operator==(const Collectible& node) {
		return m_entity == node.m_entity;
	}
};

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
	Tile* goal = nullptr;
	vector<Node> path;
	AStar* aStar = nullptr;
	AssetRef<Model> m_lamp;
	AssetRef<Model> m_sphere;
	vector<Entity*> m_lamps;
	vector<Collectible> m_keys;
	AssetRef<BasicMaterial> mat2;
	bool drawDebugLines = false;
	Player player;
	int keys = 0;
	Pointlight playerLight;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override;

	void Update(const TimeStep& time) override {
		if (GetPipeline()->m_selectedCamera == 1) {
			player.Move(time);
			playerLight.m_position = player.body.m_position;
			playerLight.m_position.y += 0.5f;
		}

		vector<Tile*>& surroundingTiles = player.GetTiles();
		for (Tile* tile : surroundingTiles) {
			if (tile->m_definition == &TileDefinition::LOCK && tile->IsSolid() && keys > 0) {
				keys--;
				GetWorld()->BreakTile(tile->m_position.x, tile->m_position.y);
			}
		}
		GetCamera()->Update(time);

		//if (KeyJustDown('O')) {
		//	for (int x = -20; x < 20; x++) {
		//		for (int y = -20; y < 20; y++) {
		//			for (int z = -2; z < 20; z++) {
		//				if (Math::RandomInt(0, 10) > 8) m_pointlights.push_back(Pointlight(Vector3((float)x, (float)y, (float)z), 5, Color::RandomPrimary()));
		//			}
		//		}
		//	}
		//	//loop(x, GetWorld()->GetBoundaries().m_size.x) {
		//	//	loop(y, GetWorld()->GetBoundaries().m_size.y) {
		//	//		if (Math::RandomInt(0, 10) > 8) m_pointlights.push_back(Pointlight(Vector3((float)x, 1.2f, (float)y), 2, Color::RandomPrimary()));
		//	//	}
		//	//}
		//}

		static float sinVal = 0;
		sinVal += 0.001f * time.GetMills();
		for (Collectible& coll : m_keys) {
			coll.m_entity->m_position.y = (Math::Sin(sinVal) / 4) + (Math::Cos(sinVal) / 16) + 0.5f;
			coll.m_entity->m_rotation.x = sinVal;
			coll.m_entity->m_rotation.y = 0.2f * sinVal;
			coll.m_entity->m_rotation.z = 0.5f * sinVal;
			coll.m_pointLight.m_position.y = (Math::Sin(sinVal) / 4) + (Math::Cos(sinVal) / 16) + 0.5f;

			if (!coll.m_removed && coll.m_entity->m_position.Distance(player.body.m_position) < 1.5f) {
				coll.m_removed = true;
				Tween* tween = Tween::To(coll.m_entity->m_scale, Vector3(0.01f, 0.01f, 0.01f), 1000);
				tween->SetEase(Ease::INELASTIC);
				tween->SetOnComplete([&] {
					delete coll.m_entity;
					Utils::RemoveFromVector(m_keys, coll);
				});
				keys++;
			}
		}

		//Vector3 cast = m_rayCast.GetGroundPosition(GetCamera());
		//m_rayCastPos = m_rayCast.GetTile();
		//
		//float x = ((float)m_rayCastPos.x + 0.5f);
		//float y = ((float)m_rayCastPos.y + 0.5f);
		//
		//m_pointlights[0].m_position.x = cast.x;
		//m_pointlights[0].m_position.y = 1.2f;
		//m_pointlights[0].m_position.z = cast.z;

		//if (GetMouse()->ButtonDown(VK_MOUSE_LEFT)) {
		//	GetWorld()->BreakTile((int)x, (int)y);
		//}

		//if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
		//	m_pointlights.push_back(Pointlight(Vector3(cast.x, 1.5f, cast.z), 25, Color(1, 0.8f, 0.8f)));
		//}
		//if (KeyJustDown('H')) {
		//	m_pointlights.push_back(Pointlight(Vector3(cast.x, 1.5f, cast.z), 25, Color::RandomPrimary()));
		//}
	}

	void RenderGeometry() override {
		//player.DebugDraw();
		GetPipeline()->GetGBuffer()->BindTextures();
		GetTileRenderer()->Begin();
		GetWorld()->Draw();
		GetTileRenderer()->End();
		GetTileRenderer()->Draw();
		GetLineRenderer()->DrawRect(Rect((float)m_rayCastPos.x + 0.5f, (float)m_rayCastPos.y + 0.5f, 1.0f, 1.0f), Color::Red());

		if (drawDebugLines) {
			Tile* tile = GetWorld()->GetTile(14, 0);
			DrawFromTileToTile(tile);

			if (goal != nullptr) {
				Tile* parentTile = goal;
				while (parentTile->m_parent) {
					GetLineRenderer()->Submit(parentTile->m_position.x + 0.5f, 1.06f, parentTile->m_position.y + 0.5f, parentTile->m_parent->m_position.x + 0.5f, 1.06f, parentTile->m_parent->m_position.y + 0.5f, Color::Blue());
					parentTile = parentTile->m_parent;
				}
			}

			if (aStar) {
				aStar->Draw();
			}
		}

		GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries(), Color::Red());
		m_ludo->DebugDraw();
		loop(i, m_pointlights.size()) {
			GetPointlightRenderer()->Submit(m_pointlights[i]);
		}

		for (Collectible& coll : m_keys) {
			GetPointlightRenderer()->Submit(coll.m_pointLight);
		}

		GetPointlightRenderer()->Submit(playerLight);

		m_geometryShader->Bind();
		m_geometryShader->Set("_Iridescence", 4);
		texIri->Bind(4);
		m_geometryShader->Set("_IridescenceStrength", 0);
		m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
		m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);
		m_geometryShader->Set("_TessellationFactor", (float)0);
		m_geometryShader->Set("_TessellationSlope", (float)0);
		m_geometryShader->Set("_TessellationShift", (float)0);
		m_geometryShader->Set("_TessellationAlpha", (float)0);
		for (Entity* e : m_lamps) e->Draw(m_geometryShader, GL_PATCHES);
		for (Collectible coll : m_keys) coll.m_entity->Draw(m_geometryShader, GL_PATCHES);
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
		ImGui::SliderFloat("Mat2 normal", &mat2->m_normalStrength, 0, 1);
		ImGui::SliderFloat("Mat2 specular", &mat2->m_specularStrength, 0, 5);
		ImGui::SliderFloat("Mat2 emission", &mat2->m_emissionStrength, 0, 5);
		ImGui::SliderFloat("metallic", &GetPipeline()->metallic, 0, 1);
		ImGui::SliderFloat("_IridescenceStrength", &GetTileRenderer()->m_scale3, 0, 1);
		//ImGui::SliderFloat("Normal", &GetTileRenderer()->m_material->m_normalStrength, 0, 10);
		if (ImGui::Button("Execute")) { m_ludo->Execute(); }
		if (ImGui::Button("Reset world")) {
			GetWorld()->Reset();
			GetWorld()->GetTile(14, 0)->SetDefinition(&TileDefinition::HOOK);
		}
	}

	void FindDeepestNode(Tile* tile, int level, float& distance, Tile*& deepestTile) {
		if (tile->m_tiles.size() == 0) return;

		float dist = Vector2(14, 0).Distance(Vector2(tile->m_position.x, tile->m_position.y));
		if (dist > distance) {
			deepestTile = tile->m_tiles[0];
		}
		distance = max(dist, distance);

		for (int i = 0; i < tile->m_tiles.size(); i++) {
			FindDeepestNode(tile->m_tiles[i], level, distance, deepestTile);
		}
	}
	void Cleanup() override;
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};