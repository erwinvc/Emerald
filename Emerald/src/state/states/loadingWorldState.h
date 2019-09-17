#pragma once

class LoadingWorldState : public State {
private:
	String m_name = "LoadingWorld";
public:
	const String& GetName() override { return m_name; }
	AssetRef<Shader> m_shader;
	CustomMesh* mesh;
	Vertex* vertices;
	uint* indices;
	int planeSizeX = 16 * 2;
	int planeSizeY = 16 * 2;
	vector<Pointlight> m_pointlights;
	float m_scale1 = 0;
	float m_scale2 = 0;
	float iridescenceStrength = 0;
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	AssetRef<Texture> texWhite;
	Vector2I m_rayCastPos;
	uint indexCount = 0;
	int count = 0;
	GroundRaycast rayCast;
	Entity* entity;
	Model* sphere;
	Mesh* sphereMesh;
	Model* turtleModel;
	Entity* turtle;

	float t1 = 3.35f;
	float t2 = 3.7f;
	float t3 = 0.0f;
	float t4 = 2;

	AssetRef<BasicMaterial> m_material;
	void Initialize() override {
		m_material = GetMaterialManager()->Create<BasicMaterial>("test");
		AssetRef<BasicMaterial> mat2 = GetMaterialManager()->Create<BasicMaterial>("test2");
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		texNoise = GetAssetManager()->Get<Texture>("Noise");
		texWhite = GetAssetManager()->Get<Texture>("White");
		m_material->SetAlbedo(texWhite);
		m_material->SetNormal(GetAssetManager()->Get<Texture>("BricksNormal"));
		mat2->SetAlbedo(texIri);
		m_shader = GetShaderManager()->Get("Geometry");
		turtleModel = GetAssetManager()->Get<Model>("dragon");
		turtleModel->SetMaterial(m_material);
		m_pointlights.push_back(Pointlight(GetCamera()->m_position, 25, Color::White()));

		sphereMesh = MeshGenerator::Sphere(20, 20);
		sphere = NEW(Model(sphereMesh));
		sphere->SetMaterial(mat2);
		entity = NEW(Entity(sphere));
		entity->m_scale = Vector3(0.05f, 0.05f, 0.05f);
		turtle = NEW(Entity(turtleModel));
		turtle->m_position.x += 8;
		turtle->m_position.z += 20;
		turtle->m_position.y += 5;
		turtle->m_rotation.y += Math::PI;
		turtle->m_scale *= 25;

		int layerVertexCount = (planeSizeX + 1);
		int totalVertexCount = layerVertexCount * 2 * planeSizeY;
		indexCount = planeSizeX * 2 * 3 * planeSizeY;
		count = indexCount;
		vertices = new Vertex[totalVertexCount];
		indices = new uint[indexCount];

		for (int y = 0; y < planeSizeY; y++) {
			int totalLayerVertexCount = y * layerVertexCount;
			int totalLayerVertexCountPlusOne = (y + 1) * layerVertexCount;
			for (int x = 0; x < layerVertexCount; x++) {
				float relativeX = (float)x / 2;
				float relativeY = (float)y / 2;
				vertices[x + totalLayerVertexCount].m_position = Vector3(relativeX, 1, relativeY);
				vertices[x + totalLayerVertexCountPlusOne].m_position = Vector3(relativeX, 1, relativeY + 0.125f);
				vertices[x + totalLayerVertexCount].m_uv = Vector2(relativeX, relativeY);
				vertices[x + totalLayerVertexCountPlusOne].m_uv = Vector2(relativeX, relativeY + 0.125f);
			}
		}

		int index = 0;
		for (int y = 0; y < planeSizeY; y++) {
			int layerSize = y * layerVertexCount;

			for (int i = 0; i < planeSizeX; i++) {
				if ((i + y) % 2 == 0) {
					indices[index++] = layerVertexCount + i + 1 + layerSize; //4
					indices[index++] = i + layerSize;						 //1
					indices[index++] = layerVertexCount + i + layerSize;	 //3

					indices[index++] = i + 1 + layerSize;					 //2
					indices[index++] = i + layerSize;						 //1
					indices[index++] = layerVertexCount + i + 1 + layerSize; //4
				} else {
					indices[index++] = i + layerSize;						 //1
					indices[index++] = layerVertexCount + i + layerSize;	 //3
					indices[index++] = i + 1 + layerSize;					 //2

					indices[index++] = i + 1 + layerSize;					 //2
					indices[index++] = layerVertexCount + i + layerSize;	 //3
					indices[index++] = layerVertexCount + i + 1 + layerSize; //4
				}
			}
		}

		mesh = new CustomMesh();
		mesh->SetMaterial(m_material);

		mesh->SetVertices(vertices, totalVertexCount);
		mesh->SetIndices(indices, indexCount);

		mesh->CalculateNormals();
		mesh->UploadMeshData();
	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);

		if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
			m_pointlights.push_back(Pointlight(Vector3(GetCamera()->m_position.x, GetCamera()->m_position.y, GetCamera()->m_position.z), 25, Color(1, 0.8f, 0.8f)));
		}

		Vector3 cast = rayCast.GetGroundPosition(1.0f);
		m_rayCastPos = rayCast.GetTile();

		m_pointlights[0].m_position.x = cast.x;
		m_pointlights[0].m_position.y = 1.2f;
		m_pointlights[0].m_position.z = cast.z;

		Vector3 groundPos = rayCast.GetGroundPosition();
		groundPos.x = Math::Round(groundPos.x + 0.5f);
		groundPos.y = 1;
		groundPos.z = Math::Round(groundPos.z + 0.5f);

		entity->m_position.x = groundPos.x;
		entity->m_position.y = 1.0f;
		entity->m_position.z = groundPos.z;

		int layerVertexCount = planeSizeY + 1;
		int index1 = (groundPos.x * 2) + ((groundPos.z * 2) * layerVertexCount);
		int index2 = index1 - 1;
		int index3 = index1 - 2;

		int index11 = index1 - layerVertexCount;
		int index12 = index2 - layerVertexCount;
		int index13 = index3 - layerVertexCount;

		int index21 = index11 - layerVertexCount;
		int index22 = index12 - layerVertexCount;
		int index23 = index13 - layerVertexCount;

		if (ButtonDown(VK_MOUSE_LEFT)) {
			if (Vertex* vertex = mesh->GetVertex(index1)) lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index2)) lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index3)) lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index11))lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index12))lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index13))lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index21))lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index22))lower(vertex);
			if (Vertex* vertex = mesh->GetVertex(index23))lower(vertex);
			mesh->CalculateNormals();
			mesh->UploadMeshData();
		}
	}

	void lower(Vertex* v) {
		v->m_position.y -= 0.02f;
		if (v->m_position.y < 0) v->m_position.y = 0;
	}
	void RenderGeometry() override {
		loop(i, m_pointlights.size()) {
			GetPointlightRenderer()->Submit(m_pointlights[i]);
		}

		m_shader->Bind();
		m_shader->Set("_Iridescence", 4);
		texIri->Bind(4);
		m_shader->Set("_IridescenceStrength", iridescenceStrength);
		m_shader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_shader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_shader->Set("_TransformationMatrix", Matrix4::Identity());
		m_shader->Set("_CameraPosition", GetCamera()->m_position);
		m_shader->Set("_TessellationFactor", (float)t1);
		m_shader->Set("_TessellationSlope", (float)t2);
		m_shader->Set("_TessellationShift", (float)t3);
		m_shader->Set("_TessellationAlpha", (float)t4);

		GetMaterialManager()->GetNullMaterial()->Bind(m_shader);
		mesh->GetMaterial()->Bind(m_shader);
		mesh->DrawCount(count, GL_PATCHES);
		turtle->Draw(m_shader, GL_PATCHES);
		GetLineRenderer()->DrawRect(Rect((float)m_rayCastPos.x + 0.5f, (float)m_rayCastPos.y + 0.5f, 1.0f, 1.0f), Color::Red());

	}
	void RenderUI() override {}
	void OnImGUI() override {
		ImGui::SliderFloat("IridescenceStrength", &iridescenceStrength, 0, 1);
		ImGui::SliderFloat("NormalStrength", &m_material->m_normalStrength, -10, 10);
		ImGui::SliderFloat("tessellationFactor", &t1, 0, 20);
		ImGui::SliderFloat("tessellationSlope", &t2, 0, 10);
		ImGui::SliderFloat("tessellationShift", &t3, 0, 0.1f);
		ImGui::SliderFloat("tessellationAlpha", &t4, 0, 16);
		ImGui::DragInt("count", &count, 1, 0, indexCount);
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};