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

		MeshData(const String& name, aiMesh* mesh, const aiScene* scene) {
			m_materialIndex = mesh->mMaterialIndex;

			m_numVertices = mesh->mNumVertices;
			m_vertices = new Vertex[m_numVertices];

			for (uint32 i = 0; i < mesh->mNumVertices; i++) {
				m_posMin.x = Math::Min(m_posMin.x, mesh->mVertices[i].x);
				m_posMin.y = Math::Min(m_posMin.y, mesh->mVertices[i].y);
				m_posMin.z = Math::Min(m_posMin.z, mesh->mVertices[i].z);
				m_posMax.x = Math::Max(m_posMax.x, mesh->mVertices[i].x);
				m_posMax.y = Math::Max(m_posMax.y, mesh->mVertices[i].y);
				m_posMax.z = Math::Max(m_posMax.z, mesh->mVertices[i].z);
				m_vertices[i].m_position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				m_vertices[i].m_normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				m_vertices[i].m_uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				m_vertices[i].m_tangents = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				m_vertices[i].m_biTangents = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}

			for (uint i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				for (uint j = 0; j < face.mNumIndices; j++)
					m_indices.push_back(face.mIndices[j]);
			}
		}
		~MeshData() {
			delete[] m_vertices;
		}
	};

	Path m_path;
	map<String, TextureLoader> m_textureData;
	vector<MeshData*> m_meshData;
	vector<MaterialData> m_materialData;
	//vector<Material*> m_materials;
	bool m_loadMaterials = false;
	int m_width = 0;
	int m_height = 0;

	void LoadMaterials(const aiScene* scene);
	String LoadTexture(int index, aiMaterial* mat, aiTextureType type);

public:
	ModelLoader(const String& name, const Path& path, bool loadMaterials = true) : AssetLoader(name), m_loadMaterials(loadMaterials), m_path(path) {}

	void ProcessNode(aiNode* node, const aiScene* scene) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshData.push_back(NEW(MeshData(m_name, mesh, scene)));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	void AsyncLoad() override;

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};