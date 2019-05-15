#include "stdafx.h"

void Model::LoadModel(const String& path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_RemoveRedundantMaterials | aiProcess_FixInfacingNormals |  aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

    ASSERT(scene && scene->mFlags != AI_SCENE_FLAGS_INCOMPLETE && scene->mRootNode, "Model failed to load");
    m_materials.reserve(scene->mNumMaterials);
    m_meshes.reserve(scene->mNumMeshes);

    for (int i = 0; i < (int)scene->mNumMaterials; i++) m_materials[i] = new Material();

    m_dir = path.substr(0, path.find_last_of('/'));
    if (scene->HasMaterials()) LoadMaterials(scene);

    ProcessNode(scene->mRootNode, scene);
}
