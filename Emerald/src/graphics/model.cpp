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
	String_t shortName = Utils::GetShortFilename(path.c_str());
	Timer timer;
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, ImportFlags);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) LOG_ERROR("[~g3DModel~x] Failed to load ~1%s", shortName);
	m_materials.reserve(scene->mNumMaterials);
	m_meshes.reserve(scene->mNumMeshes);

	for (int i = 0; i < (int)scene->mNumMaterials; i++) m_materials[i] = NEW(Material());

	m_dir = path.substr(0, path.find_last_of('/'));
	if (scene->HasMaterials()) LoadMaterials(scene);
	LOG("[~g3DModel~x] ~1%s~x loaded in %.2f MS", shortName, timer.Get());
	ProcessNode(scene->mRootNode, scene);
}
