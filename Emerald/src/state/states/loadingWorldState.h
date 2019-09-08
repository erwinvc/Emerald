#pragma once

class LoadingWorldState : public State {
private:
	String m_name = "LoadingWorld";
public:
	const String& GetName() override { return m_name; }
	AssetRef<Shader> m_shader;
	CustomMesh* mesh;
	Model* model;
	Vertex* vertices;
	uint* indices;
	int size = 16 * 2;
	int layers = 16 * 2;
	vector<Pointlight> m_pointlights;
	float m_scale1 = 0;
	float m_scale2 = 0;
	float m_scale3 = 0;
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	AssetRef<Texture> texWhite;
	uint indexCount = 0;
	int count = 0;
	GroundRaycast rayCast;
	Entity* entity;
	Model* sphere;
	Mesh* sphereMesh;
	void Initialize() override {
		AssetRef<BasicMaterial> mat = GetMaterialManager()->Create<BasicMaterial>("test");
		AssetRef<BasicMaterial> mat2 = GetMaterialManager()->Create<BasicMaterial>("test2");
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		texNoise = GetAssetManager()->Get<Texture>("Noise");
		texWhite = GetAssetManager()->Get<Texture>("White");
		mat->SetAlbedo(texWhite);
		mat->SetNormal(GetAssetManager()->Get<Texture>("BricksNormal"));
		mat2->SetAlbedo(texIri);
		m_shader = GetShaderManager()->Get("Geometry");
		model = GetAssetManager()->Get<Model>("dragon");
		//model->SetMaterial(mat);

		sphereMesh = MeshGenerator::Sphere(20, 20);
		sphere = NEW(Model(sphereMesh));
		sphere->SetMaterial(mat2);
		entity = NEW(Entity(sphere));
		entity->m_scale = Vector3(0.05f, 0.05f, 0.05f);

		int vertPerLayer = (size + 1);
		int vertCount = vertPerLayer * 2 * layers;
		indexCount = size * 2 * 3 * layers;
		vertices = new Vertex[vertCount];

		for (int i = 0; i < vertCount; i++) {
			vertices[i] = Vertex();
		}

		float segmentSize = Math::TWO_PI / (vertPerLayer - 1);
		for (int height = 0; height < layers; height++) {
			int vertOffset = height * vertPerLayer;
			int vertOffsetPlusOne = (height + 1) * vertPerLayer;
			for (int x = 0; x < vertPerLayer; x++) {
				float size = (float)x / 2;
				float y = (float)height / 2;
				vertices[x + vertOffset].m_position = Vector3(size, 1, y);
				vertices[x + vertOffsetPlusOne].m_position = Vector3(size, 1, y + 0.125f);
				vertices[x + vertOffset].m_uv = Vector2(size, y);
				vertices[x + vertOffsetPlusOne].m_uv = Vector2(size, y + 0.125f);
			}
		}

		indices = new uint[indexCount];

		int index = 0;

		for (int height = 0; height < layers; height++) {
			int layerSize = height * (size + 1);

			for (int i = 0; i < size; i++) {
				if ((i + height) % 2 == 0) {
					indices[index++] = i + layerSize;					 //1
					indices[index++] = vertPerLayer + i + layerSize;     //3
					indices[index++] = i + 1 + layerSize;				 //2

					indices[index++] = i + 1 + layerSize;				 //2
					indices[index++] = vertPerLayer + i + layerSize;	 //3
					indices[index++] = vertPerLayer + i + 1 + layerSize; //4
				} else {
					indices[index++] = vertPerLayer + i + 1 + layerSize; //4
					indices[index++] = i + layerSize;					 //1
					indices[index++] = vertPerLayer + i + layerSize;	 //3

					indices[index++] = i + 1 + layerSize;				 //2
					indices[index++] = i + layerSize;					 //1
					indices[index++] = vertPerLayer + i + 1 + layerSize; //4
				}
			}
		}

		mesh = new CustomMesh();
		mesh->SetVertices(vertices, vertCount);
		mesh->SetIndices(indices, indexCount);
		mesh->SetMaterial(mat);

		mesh->CalculateNormals();
		mesh->UploadMeshData();
	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);

		if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
			m_pointlights.push_back(Pointlight(Vector3(GetCamera()->m_position.x, GetCamera()->m_position.y, GetCamera()->m_position.z), 25, Color(1, 0.8f, 0.8f)));
		}

		Vector3 cast = rayCast.GetGroundPosition(GetCamera());

		Vector3 groundPos = rayCast.GetGroundPosition();
		groundPos.x = Math::Round(groundPos.x * 2) / 2.0f;
		groundPos.y = 1;
		groundPos.z = Math::Round(groundPos.z * 2) / 2.0f;

		entity->m_position.x = groundPos.x;
		entity->m_position.y = 1.0f;
		entity->m_position.z = groundPos.z;

		int index = (groundPos.x * 2) + ((groundPos.z * 2) * (size + 1));

		if (ButtonDown(VK_MOUSE_LEFT)) {
			Vertex* vertex = mesh->GetVertex(index);
			if (vertex) {
				vertex->m_position.y = 0;
				mesh->CalculateNormals();
				mesh->UploadMeshData();
			}
		}
	}
	void RenderGeometry() override {
		loop(i, m_pointlights.size()) {
			GetPointlightRenderer()->Submit(m_pointlights[i]);
		}

		m_shader->Bind();
		m_shader->Set("texture_iridescence", 4);
		texIri->Bind(4);
		m_shader->Set("texture_noise", 5);
		texNoise->Bind(5);
		m_shader->Set("scale1", m_scale1);
		m_shader->Set("scale2", m_scale2);
		m_shader->Set("scale3", m_scale3);
		m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
		m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());
		m_shader->Set("transformationMatrix", Matrix4::Identity());

		GetMaterialManager()->GetNullMaterial()->Bind(m_shader);
		mesh->GetMaterial()->Bind(m_shader);
		mesh->Draw(count);
		entity->Draw(m_shader);
		//model->Draw(m_shader);
		GetLineRenderer()->Submit(0, 0, 0, 1, 0, 0);
	}
	void RenderUI() override {}
	void OnImGUI() override {
		ImGui::SliderFloat("scale1", &m_scale1, -5, 5);
		ImGui::SliderFloat("scale2", &m_scale2, -5, 5);
		ImGui::SliderFloat("scale3", &m_scale3, 0, 1);
		ImGui::DragInt("count", &count, 1, 0, indexCount);
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};