#include "stdafx.h"

void Model::LoadModel(const String& path) {
	const unsigned int ImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure;

	String_t shortName = FileSystem::GetShortFilename(path.c_str());
	Timer timer;
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, ImportFlags);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("[~g3DModel~x] Failed to load ~1%s", shortName);
		return;
	}
	m_materials.reserve(scene->mNumMaterials);
	m_meshes.reserve(scene->mNumMeshes);

	for (int i = 0; i < (int)scene->mNumMaterials; i++) {
		m_materials.push_back(GetMaterialManager()->Create<BasicMaterial>(Format("%s[%d]", shortName, i)));
	}

	m_dir = path.substr(0, path.find_last_of('/'));
	if (scene->HasMaterials()) LoadMaterials(scene);
	LOG("[~g3DModel~x] ~1%s~x loaded in %.2fms", shortName, timer.Get());
	ProcessNode(scene->mRootNode, scene);
}

void Model::LoadMaterials(const aiScene *scene) {
	for (int i = 0; i < (int)scene->mNumMaterials; i++) {

		aiMaterial* mat = scene->mMaterials[i];
		if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_SPECULAR, 0, &path);
			String fullPath = m_dir + "\\" + path.C_Str();
			AssetRef<Texture> tex = GetAssetManager()->Get<Texture>(fullPath);
			if (tex.Get()) {
				m_materials[i]->SetSpecular(tex);
			} else {
				if (FileSystem::DoesFileExist(fullPath)) {
					TextureLoader loader(fullPath, fullPath);
					Texture* texture = GetAssetManager()->ForceLoad<Texture>(&loader);
					m_materials[i]->SetSpecular(texture);
				} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
			}
		}

		if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_AMBIENT, 0, &path);
			String fullPath = m_dir + "\\" + path.C_Str();
			AssetRef<Texture> tex = GetAssetManager()->Get<Texture>(fullPath);
			if (tex.Get()) {
				m_materials[i]->SetAlbedo(tex);
			} else {
				if (FileSystem::DoesFileExist(fullPath)) {
					TextureLoader loader(fullPath, fullPath);
					Texture* texture = GetAssetManager()->ForceLoad<Texture>(&loader);
					m_materials[i]->SetAlbedo(texture);
				} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
			}
		}
		if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_HEIGHT, 0, &path);
			String fullPath = m_dir + "\\" + path.C_Str();
			AssetRef<Texture> tex = GetAssetManager()->Get<Texture>(fullPath);
			if (tex.Get()) {
				m_materials[i]->SetNormal(tex);
			} else {
				if (FileSystem::DoesFileExist(fullPath)) {
					TextureLoader loader(fullPath, fullPath);
					Texture* texture = GetAssetManager()->ForceLoad<Texture>(&loader);
					m_materials[i]->SetNormal(texture);
				} else LOG("[~gTexture~x] ~rTexture does not exist at location ~1%s", fullPath.c_str());
			}
		}
	}
}