#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

void ChunkMesh::AddFace(const float face[12], BlockSide side, const glm::ivec3 position, uint8 voxel) {
	int index = 0;
	for (unsigned i = 0; i < 4; i++) {
		float x = face[index++] + position.x;
		float y = face[index++] + position.y;
		float z = face[index++] + position.z;
		m_vertices.emplace_back(glm::vec3(x, y, z), glm::vec3(i, (int)side, voxel - 1));
	}

	m_indices.push_back(m_indexIndex);
	m_indices.push_back(m_indexIndex + 1);
	m_indices.push_back(m_indexIndex + 2);
	m_indices.push_back(m_indexIndex + 2);
	m_indices.push_back(m_indexIndex + 3);
	m_indices.push_back(m_indexIndex);
	m_indexIndex += 4;
}

void ChunkMesh::GenerateMesh() {
	ManagedRef<VertexArray> vao(new VertexArray());
	ManagedRef<IndexBuffer> ibo(new IndexBuffer(m_indices.data(), m_indices.size()));

	BufferLayout layout = {
		{VertexBufferDataType::Float3, "vsPos", 0},
		{VertexBufferDataType::Float3, "vsIndex", 1}
	};

	vao->AddBuffer(NEW(VertexBuffer((float*)m_vertices.data(), m_vertices.size(), layout)));
	vao->ApplyLayouts();
	m_mesh = NEW(Mesh(vao, ibo));
	m_generated = true;
}