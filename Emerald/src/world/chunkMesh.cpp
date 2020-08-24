#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

glm::ivec2 s_uv[4] = {
	glm::ivec2(0, 0),
	glm::ivec2(1, 0),
	glm::ivec2(1, 1),
	glm::ivec2(0, 1)
};

void ChunkMesh::AddFace(const bool face[12], BlockSide side, const glm::ivec3 position, uint8 block, uint8 xMinOffset, uint8 xMaxOffset, uint8 yMinOffset, uint8 yMaxOffset, uint8 zMinOffset, uint8 zMaxOffset) {
	int index = 0;
	for (uint i = 0; i < 4; i++) {
		uint32 x = (face[index++] ? 16 - xMaxOffset : xMinOffset) + (position.x * 16);
		uint32 y = (face[index++] ? 16 - yMaxOffset : yMinOffset) + (position.y * 16);
		uint32 z = (face[index++] ? 16 - zMaxOffset : zMinOffset) + (position.z * 16);
		glm::ivec2 uv = glm::ivec2(16, 16) * s_uv[i];

		
		switch (side) {
			case BlockSide::WEST:
				if (i == 2 || i == 3) uv.y -= yMinOffset;
				else uv.y += yMaxOffset;

				if (i == 1 || i == 2) uv.x -= zMinOffset;
				else uv.x += zMaxOffset;
				break;
			case BlockSide::EAST:
				if (i == 2 || i == 3) uv.y -= yMinOffset;
				else uv.y += yMaxOffset;

				if (i == 1 || i == 2) uv.x -= zMaxOffset;
				else uv.x += zMinOffset;
				break;
			case BlockSide::SOUTH:
				if (i == 2 || i == 3) uv.y -= yMinOffset;
				else uv.y += yMaxOffset;

				if (i == 1 || i == 2) uv.x -= xMinOffset;
				else uv.x += xMaxOffset;
				break;
			case BlockSide::NORTH:
				if (i == 2 || i == 3) uv.y -= yMinOffset;
				else uv.y += yMaxOffset;

				if (i == 1 || i == 2) uv.x -= xMaxOffset;
				else uv.x += xMinOffset;
				break;
			case BlockSide::UP:
				if (i == 2 || i == 3) uv.y -= zMaxOffset;
				else uv.y += zMinOffset;
			
				if (i == 1 || i == 2) uv.x -= xMinOffset;
				else uv.x += xMaxOffset;
				break;
			case BlockSide::DOWN:
				if (i == 2 || i == 3) uv.y -= zMaxOffset;
				else uv.y += zMinOffset;
			
				if (i == 1 || i == 2) uv.x -= xMaxOffset;
				else uv.x += xMinOffset;
			break;
		}
		//
		//	DOWN,	// NEGATIVE Y
		//	UP,		// POSITIVE Y
		//	NORTH,	// NEGATIVE Z
		//	SOUTH,	// POSITIVE Z
		//	WEST,	// NEGATIVE X
		//	EAST	// POSITIVE X
		uint32 vertex = x | y << 10 | z << 20 | i << 30;
		uint32 vertex2 = (uint32)(block - 1) | (uint32)side << 12 | (int32)uv.x << 15 | (int32)uv.y << 20;
		m_vertices.emplace_back(glm::uvec2(vertex, vertex2));

		//float side2 = float((vertex2 & 0x7000u) >> 12u);


		//LOG("i %d %f | s %d %f", i, 1, side, side2);
		//m_vertices.push_back(vertex);
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
		{VertexBufferDataType::Int2, "vsPos", 0},
		//{VertexBufferDataType::Int3, "vsPos", 0},
		//{VertexBufferDataType::Float3, "vsIndex", 1}
	};

	vao->AddBuffer(NEW(VertexBuffer((const void*)m_vertices.data(), m_vertices.size(), layout)));

	vao->ApplyLayouts();
	m_mesh = NEW(Mesh(vao, ibo));
	m_generated = true;
}