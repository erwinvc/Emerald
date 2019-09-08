#include "stdafx.h"

void ModelLoader::SyncLoad(map<String, AssetBase*>& assets) {
	vector<AssetRef<Mesh>> meshes;
	meshes.reserve(m_preloadedMeshes.size());
	for (PreloadedMesh& preloadedMesh : m_preloadedMeshes) {
		ManagedRef<VertexArray> vaoModel(new VertexArray());
		vaoModel->AddBuffer(NEW(VertexBuffer((float*)preloadedMesh.m_vertices, preloadedMesh.m_numVertices, m_layout)));
		vaoModel->ApplyLayouts();

		ManagedRef<IndexBuffer> ibo(new IndexBuffer(preloadedMesh.m_indices, preloadedMesh.m_numIndices));
		Mesh* mesh = NEW(Mesh(vaoModel, ibo, GetMaterialManager()->GetNullMaterial()));
		meshes.push_back(mesh);

		delete[] preloadedMesh.m_vertices;
		delete[] preloadedMesh.m_indices;
	}

	assets[m_name] = NEW(Model(meshes));
}