#pragma once

class Model : public AssetBase {
private:
	vector<AssetRef<Mesh>> m_meshes;
	vector<AssetRef<BasicMaterial>> m_materials;
	map<String, AssetRef<Texture>> m_textures;
	String m_dir;

	void ProcessNode(aiNode* node, const aiScene* scene) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	struct Vertex {
		Vector3 m_position;
		Vector3 m_normal;
		Vector2 m_uv;
		Vector3 m_tangents;
		Vector3 m_biTangents;
	};
	Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		vector<GLfloat> texcoords;
		for (uint i = 0; i < mesh[0].mNumVertices; i++) {
			texcoords.push_back(mesh[0].mTextureCoords[0][i].x);
			texcoords.push_back(mesh[0].mTextureCoords[0][i].y);
		}

		//VAO


		BufferLayout layout = {
			{ShaderDataType::Float3, "vsPos", 0},
			{ShaderDataType::Float3, "vsNormal", 0},
			{ShaderDataType::Float2, "vsUv", 0},
			{ShaderDataType::Float3, "vsTangents", 0},
			{ShaderDataType::Float3, "vsBitangents", 0}
		};

		Vertex* vertices = new Vertex[mesh->mNumVertices];

		for (uint32 i = 0; i < mesh->mNumVertices; i++) {
			vertices[i].m_position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertices[i].m_normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			vertices[i].m_uv = Vector2(mesh[0].mTextureCoords[0][i].x, mesh[0].mTextureCoords[0][i].y);
			vertices[i].m_tangents = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertices[i].m_biTangents = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}

		ManagedRef<VertexArray> vaoModel(new VertexArray());
		vaoModel->AddBuffer(NEW(VertexBuffer((float*)vertices, mesh->mNumVertices, layout)));
		vaoModel->ApplyLayouts();

		delete[] vertices;

		//IBO
		vector<GLuint> indices;
		for (uint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiString path;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		String fullPath = m_dir + "\\" + path.C_Str();

		ManagedRef<IndexBuffer> ibo(new IndexBuffer(indices.data(), indices.size()));
		return NEW(Mesh(vaoModel, ibo, m_materials[mesh->mMaterialIndex]));
	}

	void LoadMaterials(const aiScene *scene) {
		for (int i = 0; i < (int)scene->mNumMaterials; i++) {
			aiMaterial* mat = scene->mMaterials[i];
			if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
				aiString path;
				mat->GetTexture(aiTextureType_AMBIENT, 0, &path);
				String fullPath = m_dir + path.C_Str();
				if (m_textures[fullPath]) {
					m_materials[i]->SetAlbedo(m_textures[fullPath]);
				} else {
					if (FileSystem::DoesFileExist(fullPath)) {
						Texture* texture = NEW(Texture(fullPath));
						m_materials[i]->SetAlbedo(texture);
						m_textures.emplace(fullPath, texture);
					} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
				}
			}
			if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
				aiString path;
				mat->GetTexture(aiTextureType_HEIGHT, 0, &path);
				String fullPath = m_dir + "\\" + path.C_Str();
				if (m_textures[fullPath]) {
					m_materials[i]->SetNormal(m_textures[fullPath]);
				} else {
					if (FileSystem::DoesFileExist(fullPath)) {
						Texture* texture = NEW(Texture(fullPath));
						m_materials[i]->SetNormal(texture);
						m_textures.emplace(fullPath, texture);
					} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
				}
			}
		}
	}
	void LoadModel(const String& path);

public:
	Model(vector<AssetRef<Mesh>> meshes) : m_meshes(meshes) {}
	Model(AssetRef<Mesh> mesh) : m_meshes({mesh}) {}
	~Model() {}

	vector<AssetRef<Mesh>> GetMeshes() { return m_meshes; }


	void SetMaterial(AssetRef<BasicMaterial> material) {
		for (auto& mesh : m_meshes) {
			mesh->SetMaterial(material);
		}
	}

	void Draw(AssetRef<Shader> shader, uint mode = GL_TRIANGLES) {
		for (auto& mesh : m_meshes) {
			mesh->GetMaterial()->Bind(shader);
			mesh->Draw(mode);
		}
	}

	void DrawIndex(AssetRef<Shader> shader, int index) {
		m_meshes[index]->GetMaterial()->Bind(shader);
		m_meshes[index]->Draw();

	}
};