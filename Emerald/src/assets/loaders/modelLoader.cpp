#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

void ModelLoader::LoadMaterials(const aiScene* scene) {
	int textureCounts[12] = { 0 };
	for (int i = 0; i < (int)scene->mNumMaterials; i++) {
		aiMaterial* mat = scene->mMaterials[i];

		for (int i = 0; i < 12; i++) {
			textureCounts[i] += mat->GetTextureCount((aiTextureType)i);
		}

		m_materialData[i].m_roughness = LoadTexture(i, mat, aiTextureType_SHININESS);
		m_materialData[i].m_metallic = LoadTexture(i, mat, aiTextureType_AMBIENT);
		m_materialData[i].m_albedo = LoadTexture(i, mat, aiTextureType_DIFFUSE);
		m_materialData[i].m_normal = LoadTexture(i, mat, aiTextureType_HEIGHT);
	}

	const String_t lookupTable[] = {
		"NONE","DIFFUSE","SPECULAR","AMBIENT","EMISSIVE","HEIGHT","NORMALS",
		"SHININESS","OPACITY","DISPLACEMENT","LIGHTMAP","REFLECTION","UNKNOWN" };

	LOG("Model ~1%s~x contains: ", m_name.c_str());
	for (int i = 0; i < 12; i++) {
		LOG(textureCounts[i] > 0 ? "	%d %s" : "	~1%d %s", textureCounts[i], lookupTable[i]);
	}
}

String ModelLoader::LoadTexture(int index, aiMaterial* mat, aiTextureType type) {
	if (mat->GetTextureCount(type) > 0) {
		aiString path;
		mat->GetTexture(type, 0, &path);
		String fullPath = m_path.GetDirectory() + "\\" + path.C_Str();
		if (m_textureData.find(fullPath) == m_textureData.end()) {
			if (FileSystem::DoesFileExist(fullPath)) {
				TextureParameters params;
				switch (type) {
					case aiTextureType_SHININESS:
						params = TextureParameters(INT_RED);
						break;
					case  aiTextureType_AMBIENT:
						params = TextureParameters(INT_RED);
						break;
					case aiTextureType_DIFFUSE:
						params = TextureParameters(INT_SRGBA);
						break;
					case aiTextureType_HEIGHT:
						params = TextureParameters(INT_RGB);
						break;
				}
				m_textureData[fullPath] = TextureLoader(fullPath, fullPath, true, params);
				m_textureData[fullPath].AsyncLoad();
				return fullPath;
			} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
		} else return fullPath;
	}
	return "";
}

void ModelLoader::AsyncLoad() {
	if (!m_path.Exists()) {
		LOG_ERROR("[~gModel~x] model ~1%s~x does not exist!", m_path.GetFullPath().c_str());
		return;
	}
	Timer timer;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_path.GetFullPath().c_str(), m_importFlags);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("[~gModel~x] model ~1%s~x failed to load: ~1%s", m_name.c_str(), importer.GetErrorString());
		return;
	}

	m_meshData.reserve(scene->mNumMeshes);
	m_materialData.reserve(scene->mNumMaterials);
	for (int i = 0; i < (int)scene->mNumMaterials; i++) {
		Material* mat = GetMaterialManager()->Create(Format("%s_%s", m_name.c_str(), scene->mMaterials[i]->GetName().C_Str()));
		m_materialData.push_back(MaterialData(mat));
	}

	if (m_loadMaterials && scene->HasMaterials()) LoadMaterials(scene);

	ProcessNode(scene->mRootNode, scene);
	LOG("[~gModel~x] loaded ~1%s~x in %.2fms", m_name.c_str(), timer.Get());
}

void ModelLoader::SyncLoad(AssetManager* manager) {
	vector<AssetRef<Mesh>> meshes;
	meshes.reserve(m_meshData.size());

	for (auto& textureData : m_textureData) {
		textureData.second.SyncLoad(GetAssetManager());
	}

	for (auto& materialData : m_materialData) {
		if (materialData.m_roughness != "") materialData.m_material->SetRoughnessIfAvailable(GetAssetManager()->Get<Texture>(materialData.m_roughness));
		if (materialData.m_metallic != "") materialData.m_material->SetMetallicIfAvailable(GetAssetManager()->Get<Texture>(materialData.m_metallic));
		if (materialData.m_albedo != "") materialData.m_material->SetAlbedoIfAvailable(GetAssetManager()->Get<Texture>(materialData.m_albedo));
		if (materialData.m_normal != "") materialData.m_material->SetNormalIfAvailable(GetAssetManager()->Get<Texture>(materialData.m_normal));
	}

	for (auto& meshData : m_meshData) {
		ManagedRef<VertexArray> vaoModel(NEW(VertexArray()));
		VertexBuffer* buff = NEW(VertexBuffer((float*)meshData->m_vertices, meshData->m_numVertices, m_layout));
		vaoModel->AddBuffer(ManagedRef<VertexBuffer>(buff));
		vaoModel->ApplyLayouts();

		Mesh* mesh = NEW(Mesh(vaoModel, NEW(IndexBuffer(meshData->m_indices.data(), meshData->m_indices.size())), m_materialData[meshData->m_materialIndex].m_material));

		float sizeX = Math::Abs(meshData->m_posMax.x - meshData->m_posMin.x) / 2;
		float sizeY = Math::Abs(meshData->m_posMax.y - meshData->m_posMin.y) / 2;
		float sizeZ = Math::Abs(meshData->m_posMax.z - meshData->m_posMin.z) / 2;
		mesh->m_size = glm::vec3(sizeX, sizeY, sizeZ);

		float posX = (meshData->m_posMin.x + meshData->m_posMax.x) / 2.0f;
		float posY = (meshData->m_posMin.y + meshData->m_posMax.y) / 2.0f;
		float posZ = (meshData->m_posMin.z + meshData->m_posMax.z) / 2.0f;
		mesh->m_center = glm::vec3(posX, posY, posZ);
		meshes.push_back(mesh);
		DELETE(meshData);
	}

	manager->AddAsset<Model>(m_name, NEW(Model(meshes)));
}