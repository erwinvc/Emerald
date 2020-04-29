#include "stdafx.h"

Mesh::Mesh() : m_vao(nullptr), m_ibo(nullptr) {
	m_material = GetMaterialManager()->GetBasicMaterial(GetShaderManager()->Get("Geometry"));
}
Mesh::Mesh(ManagedRef<VertexArray> vao, ManagedRef<IndexBuffer> ibo) : m_vao(vao), m_ibo(ibo) {
	m_material = GetMaterialManager()->GetBasicMaterial(GetShaderManager()->Get("Geometry"));
}