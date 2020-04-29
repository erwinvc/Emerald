#pragma once

//class CustomMesh : public Mesh {
//private:
//	struct Face {
//		Vertex* m_vertex1;
//		Vertex* m_vertex2;
//		Vertex* m_vertex3;
//
//		Face(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3) : m_vertex1(vertex1), m_vertex2(vertex2), m_vertex3(vertex3) {}
//	};
//
//	ManagedRef<VertexBuffer> m_vbo;
//
//	vector<Face> m_faces;
//	uint m_faceCount = 0;
//
//	Vertex* m_vertices = nullptr;
//	uint m_vertexCount = 0;
//
//	uint* m_indices = nullptr;
//	uint m_indexCount = 0;
//
//	const BufferLayout m_layout = {
//		{VertexBufferDataType::Float3, "vsPos", 0},
//		{VertexBufferDataType::Float3, "vsNormal", 0},
//		{VertexBufferDataType::Float2, "vsUv", 0},
//		{VertexBufferDataType::Float3, "vsTangents", 0},
//		{VertexBufferDataType::Float3, "vsBitangents", 0}
//	};
//
//	void GenerateFaces() {
//		m_faceCount = m_indexCount / 3;
//		m_faces.clear();
//		m_faces.reserve(m_faceCount);
//
//		for (uint i = 0; i < m_faceCount; i++) {
//			int index = i * 3;
//			Vertex* v1 = &m_vertices[m_indices[index]];
//			Vertex* v2 = &m_vertices[m_indices[index + 1]];
//			Vertex* v3 = &m_vertices[m_indices[index + 2]];
//			m_faces.emplace_back(v1, v2, v3);
//		}
//	}
//
//public:
//	CustomMesh() : Mesh() {
//		m_vao = NEW(VertexArray());
//		m_vbo = NEW(VertexBuffer((float*)nullptr, 0, m_layout));
//		m_vao->AddBuffer(m_vbo);
//		m_vao->ApplyLayouts();
//
//		m_ibo = NEW(IndexBuffer(nullptr, 0));
//	}
//
//	~CustomMesh() {
//		if (m_vertices) delete[] m_vertices;
//		if (m_indices) delete[] m_indices;
//	}
//
//	inline Vertex* GetVertex(int index) {
//		if (Math::Within<int>(index, 0, m_vertexCount)) return &m_vertices[index];
//		return nullptr;
//	}
//
//	void SetVertices(Vertex* vertices, uint count) {
//		m_vertexCount = count;
//
//		if (m_vertices) delete[] m_vertices;
//		m_vertices = new Vertex[count];
//
//		memcpy(m_vertices, vertices, count * sizeof(Vertex));
//	}
//
//	void SetIndices(uint* indices, uint count) {
//		if (count % 3 != 0) LOG_ERROR("[~gMesh~x] index count is not a mod of 3");
//		m_indexCount = count;
//
//		if (m_indices) delete[] m_indices;
//		m_indices = new uint[count];
//
//		memcpy(m_indices, indices, count * sizeof(uint));
//
//		GenerateFaces();
//	}
//
//	void UploadMeshData() {
//		m_vbo->SetData(m_vertices, m_vertexCount);
//		m_ibo->SetData(m_indices, m_indexCount);
//	}
//
//	void CalculateTangents(Vertex* v0, Vertex* v1, Vertex* v2) {
//		glm::vec3 delatPos1 = v1->m_position - v0->m_position;
//		glm::vec3 delatPos2 = v2->m_position - v0->m_position;
//		glm::vec2 deltaUv1 = v1->m_uv - v0->m_uv;
//		glm::vec2 deltaUv2 = v2->m_uv - v0->m_uv;
//
//		float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
//
//		delatPos1 *= deltaUv2.y;
//		delatPos2 *= deltaUv1.y;
//		glm::vec3 tangent = delatPos1 - delatPos2;
//		tangent *= r;
//		v0->m_tangent += tangent;
//		v1->m_tangent += tangent;
//		v2->m_tangent += tangent;
//	}
//
//	void CalculateNormals() {
//		for (uint i = 0; i < m_vertexCount; i++) {
//			m_vertices[i].m_normal = glm::vec3();
//			m_vertices[i].m_tangent = glm::vec3();
//		}
//
//		for (uint i = 0; i < m_faceCount; i++) {
//			Face& face = m_faces[i];
//
//			glm::vec3 normal;
//			glm::vec3 U = face.m_vertex2->m_position - face.m_vertex1->m_position;
//			glm::vec3 V = face.m_vertex3->m_position - face.m_vertex1->m_position;
//
//			normal.x = (U.y * V.z) - (U.z * V.y);
//			normal.y = (U.z * V.x) - (U.x * V.z);
//			normal.z = (U.x * V.y) - (U.y * V.x);
//
//			face.m_vertex1->m_normal += normal;
//			face.m_vertex2->m_normal += normal;
//			face.m_vertex3->m_normal += normal;
//
//			CalculateTangents(face.m_vertex1, face.m_vertex2, face.m_vertex3);
//		}
//
//		for (uint i = 0; i < m_vertexCount; i++) {
//			m_vertices[i].m_normal = glm::normalize(m_vertices[i].m_normal);
//			m_vertices[i].m_tangent = glm::normalize(m_vertices[i].m_tangent);
//		}
//	}
//};
//
//
