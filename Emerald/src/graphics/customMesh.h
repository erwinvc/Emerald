#pragma once

class CustomMesh : public Mesh {
private:
	struct Face {
		Vertex* m_vertex1;
		Vertex* m_vertex2;
		Vertex* m_vertex3;

		Face(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3) : m_vertex1(vertex1), m_vertex2(vertex2), m_vertex3(vertex3) {}
	};

	Vertex* m_vertices = nullptr;
	uint m_vertexCount = 0;

	uint* m_indices = nullptr;
	uint m_indexCount = 0;

	vector<Face> m_faces;
	uint m_faceCount = 0;

	const BufferLayout m_layout = {
		{ShaderDataType::Float3, "vsPos", 0},
		{ShaderDataType::Float3, "vsNormal", 0},
		{ShaderDataType::Float2, "vsUv", 0},
		{ShaderDataType::Float3, "vsTangents", 0},
		{ShaderDataType::Float3, "vsBitangents", 0}
	};

	void GenerateFaces() {
		m_faceCount = m_indexCount / 3;
		m_faces.clear();
		m_faces.reserve(m_faceCount);

		for (uint i = 0; i < m_faceCount; i++) {
			int index = i * 3;
			Vertex* v1 = &m_vertices[m_indices[index]];
			Vertex* v2 = &m_vertices[m_indices[index + 1]];
			Vertex* v3 = &m_vertices[m_indices[index + 2]];
			m_faces.emplace_back(v1, v2, v3);

		}
	}

public:
	CustomMesh() : Mesh() {
		m_vao = NEW(VertexArray());
		m_vao->AddBuffer(NEW(VertexBuffer((float*)nullptr, 0, m_layout)));
		m_vao->ApplyLayouts();

		m_ibo = NEW(IndexBuffer(nullptr, 0));
	}

	void SetVertices(Vertex* vertices, uint count) {
		if (m_vertices) delete[] m_vertices;
		m_vertices = new Vertex[count];

		memcpy(m_vertices, vertices, count * sizeof(Vertex));

		m_vertexCount = count;
	}

	inline Vertex* GetVertex(int index) { if (Math::Within<int>(index, 0, m_vertexCount)) return &m_vertices[index]; return nullptr; }

	void SetIndices(uint* indices, uint count) {
		if (count % 3 != 0)LOG_ERROR("[~gMesh~x] index count is not a mod of 3");
		if (m_indices) delete[] m_indices;
		m_indices = new uint[count];

		memcpy(m_indices, indices, count * sizeof(uint));

		m_indexCount = count;

		GenerateFaces();
	}

	void UploadMeshData() {
		m_vao->GetBuffer(0)->SetData(m_vertices, m_vertexCount);
		m_ibo->SetData(m_indices, m_indexCount);
	}

	void CalculateTangents(Vertex* v0, Vertex* v1, Vertex* v2) {
		Vector3 delatPos1 = v1->m_position - v0->m_position;
		Vector3 delatPos2 = v2->m_position - v0->m_position;
		Vector2 uv0 = v0->m_uv;
		Vector2 uv1 = v1->m_uv;
		Vector2 uv2 = v2->m_uv;
		Vector2 deltaUv1 = uv1 - uv0;
		Vector2 deltaUv2 = uv2 - uv0;

		float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
		delatPos1 *= deltaUv2.y;
		delatPos2 *= deltaUv1.y;
		Vector3 tangent = delatPos1 - delatPos2;
		tangent *= r;
		v0->m_tangent += tangent;
		v1->m_tangent += tangent;
		v2->m_tangent += tangent;
	}

	void CalculateNormals() {
		for (uint i = 0; i < m_vertexCount; i++) {
			m_vertices[i].m_normal = Vector3();
			m_vertices[i].m_tangent = Vector3();
		}

		for (uint i = 0; i < m_faceCount; i++) {
			Face& face = m_faces[i];

			Vector3 normal;
			Vector3 U = face.m_vertex2->m_position - face.m_vertex1->m_position;
			Vector3 V = face.m_vertex3->m_position - face.m_vertex1->m_position;

			normal.x = (U.y * V.z) - (U.z * V.y);
			normal.y = (U.z * V.x) - (U.x * V.z);
			normal.z = (U.x * V.y) - (U.y * V.x);

			face.m_vertex1->m_normal += normal;
			face.m_vertex2->m_normal += normal;
			face.m_vertex3->m_normal += normal;

			CalculateTangents(face.m_vertex1, face.m_vertex2, face.m_vertex3);
		}

		for (uint i = 0; i < m_vertexCount; i++) {
			Vertex* v = &m_vertices[i];

			v->m_normal.Normalize();

			//v->m_tangent = Vector3();
			//v->m_biTangent = Vector3();
			//
			//Vector3 c1 = v->m_normal.Cross(Vector3::ZAxis());
			//Vector3 c2 = v->m_normal.Cross(Vector3::YAxis());

			//if (c1.Magnitude() > c2.Magnitude()) {
			//	v->m_tangent = c1.Normalize();
			//} else {
			//	v->m_tangent = c2.Normalize();
			//}

			//v->m_biTangent = v->m_tangent.Cross(v->m_normal);
		}
	}
};