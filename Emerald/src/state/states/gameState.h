#pragma once

class GameState : public State {
private:
	World* m_world;
	AssetRef<Shader> m_geometryShader;
	Texture* texIri;
	AssetRef<Model> m_model;
	AssetRef<Model> m_turtle;
	Entity* m_entities[4];
	Entity* m_turtleEntity;
public:
	GameState() : State("Game") {}

	void Initialize() override {
		texIri = GetAssetManager()->Get<Texture>("Iridescence");
		m_geometryShader = GetShaderManager()->Get("Geometry");

		m_world = new World();
		m_model = GetAssetManager()->Get<Model>("Sphere");
		m_turtle = GetAssetManager()->Get<Model>("Turtle");
		m_turtleEntity = new Entity(m_turtle);
		//Material* mat = GetMaterialManager()->Create("Test");
		//mat->SetAlbedo(texIri);
		//mat->SetRoughness(GetTextureManager()->GetBlackTexture());
		//m_turtle->SetMaterial(mat);
		//m_model->SetMaterial(mat);
		for (int i = 0; i < 4; i++) {
			m_entities[i] = new Entity(m_model);
		}
		m_turtleEntity->transform.position.y += 10;
		m_turtleEntity->transform.size = glm::vec3(10, 10, 10);
	}


	void Update(const TimeStep& time) override {
		Camera::active->Update(time);
		m_world->Update(time);
	}
	void RenderGeometry(HDRPipeline* pipeline) override {

		m_geometryShader->Bind();
		texIri->Bind(5);

		CornerRayPositions positions = Camera::GetCornerRays(0);
		m_entities[0]->transform.position = positions.TL;
		m_entities[1]->transform.position = positions.TR;
		m_entities[2]->transform.position = positions.BR;
		m_entities[3]->transform.position = positions.BL;
		
		for (int i = 0; i < 4; i++) {
			m_entities[i]->Draw();
		}
		m_entities[0]->Draw();
		m_turtleEntity->Draw();

	}

	//void RenderUI() override {
	//}

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