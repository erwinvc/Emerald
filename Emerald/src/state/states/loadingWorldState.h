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
	int size = 10;
	int layers = 4;

	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	void Initialize() override {
		AssetRef<BasicMaterial> mat = GetMaterialManager()->Create<BasicMaterial>("test");
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		mat->SetAlbedo(texIri);
		texNoise = GetAssetManager()->Get<Texture>("Noise");
		m_shader = GetShaderManager()->Get("Geometry");
		model = GetAssetManager()->Get<Model>("dragon");
		model->SetMaterial(mat);
		int vertPerLayer = (size + 1);
		int vertCount = vertPerLayer * 2 * layers;
		int triCount = size * 2 * 3 * layers;
		vertices = new Vertex[vertCount];

		for (int i = 0; i < vertCount; i++) {
			vertices[i] = Vertex();
		}

		for (int height = 0; height < layers; height++) {
			int vertOffset = height * vertPerLayer;
			int vertOffsetPlusOne = (height + 1) * vertPerLayer;
			for (int i = 0; i < vertPerLayer; i++) {
				vertices[i + vertOffset].m_position = Vector3(Math::Cos(i), height, Math::Sin(i));
				vertices[i + vertOffsetPlusOne].m_position = Vector3(Math::Cos(i), height + 1, Math::Sin(i));
				vertices[i + vertOffset].m_uv = Vector2(i, height + 1);
				vertices[i + vertOffsetPlusOne].m_uv = Vector2(i, height);
			}
		}

		indices = new uint[triCount];

		int index = 0;

		for (int height = 0; height < layers; height++) {
			int layerSize = height * (size + 1);

			for (int i = 0; i < size; i++) {
				indices[index++] = i + layerSize;
				indices[index++] = (size + 1) + i + layerSize;
				indices[index++] = i + 1 + layerSize;

				indices[index++] = (size + 1) + i + layerSize;
				indices[index++] = (size + 1) + i + 1 + layerSize;
				indices[index++] = i + 1 + layerSize;
			}
		}
		mesh = new CustomMesh();
		mesh->SetIndices(indices, triCount);
		mesh->SetVertices(vertices, vertCount);
	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
	}
	void RenderGeometry() override {
		m_shader->Bind();
		m_shader->Set("texture_iridescence", 4);
		texIri->Bind(4);
		m_shader->Set("texture_noise", 5);
		texNoise->Bind(5);
		m_shader->Set("scale1", 0);
		m_shader->Set("scale2", 5);
		m_shader->Set("scale3", 0);
		m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
		m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());

		//GetMaterialManager()->GetNullMaterial()->Bind(m_shader);
		//mesh->Draw();
		model->Draw(m_shader);
		GetLineRenderer()->Submit(0, 0, 0, 1, 0, 0);
	}
	void RenderUI() override {}
	void OnImGUI() override {}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};