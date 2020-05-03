#include "stdafx.h"

//void Model::LoadModel(const String& path) {
//	const unsigned int ImportFlags =
//		aiProcess_CalcTangentSpace |
//		aiProcess_Triangulate |
//		aiProcess_SortByPType |
//		aiProcess_GenNormals |
//		aiProcess_OptimizeMeshes |
//		aiProcess_Debone |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_ValidateDataStructure;
//
//	String_t shortName = FileSystem::GetShortFilename(path.c_str());
//	Timer timer;
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile(path, ImportFlags);
//	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//		LOG_ERROR("[~g3DModel~x] Failed to load ~1%s", shortName);
//		return;
//	}
//	m_materials.reserve(scene->mNumMaterials);
//	m_meshes.reserve(scene->mNumMeshes);
//
//	for (int i = 0; i < (int)scene->mNumMaterials; i++) {
//		Material* mat = GetMaterialManager()->Create(Format("%s_%s", shortName, scene->mMaterials[i]->GetName().C_Str()));
//		m_materials.push_back(mat);
//	}
//
//	m_dir = path.substr(0, path.find_last_of('/'));
//	m_name = shortName;
//	if (scene->HasMaterials()) LoadMaterials(scene);
//	LOG("%d %d", scene->mNumMeshes, scene->mNumMaterials);
//	LOG("[~g3DModel~x] ~1%s~x loaded in %.2fms", shortName, timer.Get());
//	ProcessNode(scene->mRootNode, scene);
//}
//
//void Model::LoadMaterials(const aiScene* scene) {
//
//	int textureCounts[12] = { 0 };
//	for (int i = 0; i < (int)scene->mNumMaterials; i++) {
//		aiMaterial* mat = scene->mMaterials[i];
//
//		for (int i = 0; i < 12; i++) {
//			textureCounts[i] += mat->GetTextureCount((aiTextureType)i);
//		}
//
//		Texture* tempTex = nullptr;
//		if (LoadTexture(i, mat, aiTextureType_SHININESS, tempTex))	m_materials[i]->SetRoughnessIfAvailable(tempTex);
//		if (LoadTexture(i, mat, aiTextureType_DIFFUSE, tempTex))	m_materials[i]->SetAlbedoIfAvailable(tempTex);
//		if (LoadTexture(i, mat, aiTextureType_HEIGHT, tempTex))		m_materials[i]->SetNormalIfAvailable(tempTex);
//		if (LoadTexture(i, mat, aiTextureType_AMBIENT, tempTex))	m_materials[i]->SetMetallicIfAvailable(tempTex);
//	}
//
//	const String_t lookupTable[] = {
//		"NONE","DIFFUSE","SPECULAR","AMBIENT","EMISSIVE","HEIGHT","NORMALS",
//		"SHININESS","OPACITY","DISPLACEMENT","LIGHTMAP","REFLECTION","UNKNOWN" };
//
//	LOG("Model ~1%s~x contains: ", m_name.c_str());
//	for (int i = 0; i < 12; i++) {
//		LOG(textureCounts[i] > 0 ? "	%d %s" : "	~1%d %s", textureCounts[i], lookupTable[i]);
//	}
//}
//
//bool Model::LoadTexture(int index, aiMaterial* mat, aiTextureType type, Texture*& texture) {
//	if (mat->GetTextureCount(type) > 0) {
//		aiString path;
//		mat->GetTexture(type, 0, &path);
//		LOG("%s %s", m_dir.c_str(), path.C_Str());
//		String fullPath = m_dir + "\\" + path.C_Str();
//		AssetRef<Texture> tex = GetAssetManager()->Get<Texture>(fullPath);
//		if (tex.Get()) {
//			texture = tex;
//			return true;
//		} else {
//			if (FileSystem::DoesFileExist(fullPath)) {
//				TextureLoader loader(fullPath, fullPath, true);
//				tex = GetAssetManager()->ForceLoad<Texture>(&loader);
//				texture = tex;
//				return true;
//			} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
//		}
//	}
//	return false;
//}