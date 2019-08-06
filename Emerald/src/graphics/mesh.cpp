#include "stdafx.h"


Mesh::Mesh(ManagedRef<VertexArray> vao, ManagedRef<IndexBuffer> ibo) : m_vao(vao), m_ibo(ibo), m_material(GetMaterialManager()->GetNullMaterial()) {}