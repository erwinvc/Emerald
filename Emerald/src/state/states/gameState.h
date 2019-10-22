#pragma once

class GameState : public State {
private:
	World* m_world;
	String m_name = "Game";
	AssetRef<Shader> m_geometryShader;
	Texture* texIri;
	AssetRef<Model> m_model;
	AssetRef<Model> m_turtle;
	Entity* m_entities[4];
	Entity* m_turtleEntity;
public:
	const String& GetName() override { return m_name; }
	void Initialize() override {
		texIri = GetAssetManager()->Get<Texture>("Iridescence");
		m_geometryShader = GetShaderManager()->Get("Geometry");

		m_world = new World();
		m_model = GetAssetManager()->Get<Model>("Sphere");
		m_turtle = GetAssetManager()->Get<Model>("Turtle");
		m_turtleEntity = new Entity(m_turtle);
		BasicMaterial* mat = GetMaterialManager()->Create<BasicMaterial>("Test");
		mat->SetAlbedo(texIri);
		mat->SetSpecular(GetTextureManager()->GetBlackTexture());
		//m_turtle->SetMaterial(mat);
		//m_model->SetMaterial(mat);
		for (int i = 0; i < 4; i++) {
			m_entities[i] = new Entity(m_model);
		}
		m_turtleEntity->m_position.y += 10;
		m_turtleEntity->m_scale = Vector3(10, 10, 10);
	}


	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
		m_world->Update(time);
	}
	void RenderGeometry() override {
		m_world->RenderGeometry();

		m_geometryShader->Bind();
		m_geometryShader->Set("_Iridescence", 5);
		texIri->Bind(5);
		m_geometryShader->Set("_IridescenceStrength", 0);
		m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
		m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);

		CornerRayPositions positions = Camera::GetCornerRays(0);
		m_entities[0]->m_position = positions.TL;
		m_entities[1]->m_position = positions.TR;
		m_entities[2]->m_position = positions.BR;
		m_entities[3]->m_position = positions.BL;
		
		for (int i = 0; i < 4; i++) {
			m_entities[i]->Draw(m_geometryShader);
		}
		m_entities[0]->Draw(m_geometryShader);
		m_turtleEntity->Draw(m_geometryShader);
	}

	void RenderUI() override {
	}

	void OnStateImGUI() override {}
	void OnImGUI() override {
	}

	void Cleanup() override {
		delete m_world;
	}
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};