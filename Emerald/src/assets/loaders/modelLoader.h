#pragma once

class ModelLoader : public AssetLoader {
private:
	const BufferLayout m_layout = {
		{VertexBufferDataType::Float3, "vsPos", 0},
		{VertexBufferDataType::Float3, "vsNormal", 0},
		{VertexBufferDataType::Float2, "vsUv", 0},
		{VertexBufferDataType::Float3, "vsTangents", 0},
		{VertexBufferDataType::Float3, "vsBitangents", 0}
	};

	const unsigned int m_importFlags =
		aiProcess_RemoveRedundantMaterials |
		//aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_Debone |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_FindDegenerates |
		aiProcess_FindInstances |
		aiProcess_FindInvalidData |
		aiProcess_ValidateDataStructure;

	struct Vertex {
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_uv;
		glm::vec3 m_tangents;
		glm::vec3 m_biTangents;
	};

	struct MaterialData {
		Material* m_material;
		String m_roughness = "";
		String m_metallic = "";
		String m_albedo = "";
		String m_normal = "";
		String m_emission = "";

		MaterialData() {}
		MaterialData(Material* mat) : m_material(mat) {}
	};

	struct MeshData {
		Vertex* m_vertices;
		vector<uint> m_indices;
		glm::vec3 m_posMin = glm::vec3(Math::MAX_FLOAT, Math::MAX_FLOAT, Math::MAX_FLOAT);
		glm::vec3 m_posMax = glm::vec3(Math::MIN_FLOAT, Math::MIN_FLOAT, Math::MIN_FLOAT);
		int m_numVertices = 0;
		int m_materialIndex = 0;

		MeshData(const String& name, aiMesh* mesh, const aiScene* scene);
		~MeshData();
	};

	Path m_path;
	map<String, TextureLoader> m_textureData;
	vector<MeshData*> m_meshData;
	vector<MaterialData> m_materialData;
	bool m_loadMaterials = false;
	int m_width = 0;
	int m_height = 0;

	void LoadMaterials(const aiScene* scene);
	String LoadTexture(int index, aiMaterial* mat, aiTextureType type);

public:
	ModelLoader(const String& name, const Path& path, bool loadMaterials = true) : AssetLoader(name), m_loadMaterials(loadMaterials), m_path(path) {}

	void ProcessNode(aiNode* node, const aiScene* scene);
	void AsyncLoad() override;
	void SyncLoad(AssetManager* manager) override;
	float GetProgress() override;
};