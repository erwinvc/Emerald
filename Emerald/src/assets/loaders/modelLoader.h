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

	const unsigned int ImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure;

	struct PreloadedMesh {
		Vertex* m_vertices;
		GLuint* m_indices;
		int m_numVertices = 0;
		int m_numIndices = 0;

		PreloadedMesh(const String& name, aiMesh *mesh, const aiScene *scene) {
			vector<GLfloat> texcoords;
			for (uint i = 0; i < mesh[0].mNumVertices; i++) {
				texcoords.push_back(mesh[0].mTextureCoords[0][i].x);
				texcoords.push_back(mesh[0].mTextureCoords[0][i].y);
			}

			m_numVertices = mesh->mNumVertices;
			m_numIndices = mesh->mNumFaces * 3;
			m_vertices = new Vertex[m_numVertices];
			m_indices = new GLuint[m_numIndices];

			for (uint32 i = 0; i < mesh->mNumVertices; i++) {
				m_vertices[i].m_position = Vector3(mesh->mVertices[i]);
				m_vertices[i].m_normal = Vector3(mesh->mNormals[i]);
				m_vertices[i].m_uv = Vector2(mesh[0].mTextureCoords[0][i]);
				m_vertices[i].m_tangent = Vector3(mesh->mTangents[i]);
				m_vertices[i].m_biTangent = Vector3(mesh->mBitangents[i]);
			}

			for (uint i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				if (mesh->mFaces[i].mNumIndices == 3) {
					m_indices[i * 3] = mesh->mFaces[i].mIndices[0];
					m_indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
					m_indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
				} else LOG_ERROR("[~gModel~x] Strange amount of faces (%d) in model ~1%s", mesh->mFaces[i].mNumIndices, name.c_str());
			}
		}
	};

	String m_file;
	String m_dir;
	vector<PreloadedMesh> m_preloadedMeshes;
	bool m_loadMaterials = false;
	int m_width = 0;
	int m_height = 0;
public:
	ModelLoader(const String& name, const String& file, bool loadMaterials = false) : AssetLoader(name), m_loadMaterials(loadMaterials), m_file(file) {}

	void ProcessNode(aiNode* node, const aiScene* scene) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_preloadedMeshes.push_back(PreloadedMesh(m_name, mesh, scene));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}


	void AsyncLoad() override {
		if (!FileSystem::DoesFileExist(m_file)) {
			LOG_ERROR("[~gModel~x] file at ~1%s~x does not exist!", m_file.c_str());
			return;
		}

		Timer timer;
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(m_file, ImportFlags);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LOG_ERROR("[~gModel~x] Failed to load ~1%s", m_name.c_str());
			return;
		}

		m_preloadedMeshes.reserve(scene->mNumMeshes);
		ProcessNode(scene->mRootNode, scene);

		LOG("[~gModel~x] loaded ~1%s~x in %.2fms", m_name.c_str(), timer.Get());
	}
	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};