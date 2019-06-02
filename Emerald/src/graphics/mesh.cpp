#include "stdafx.h"


Mesh::Mesh(shared_ptr<VertexArray> vao, shared_ptr<IndexBuffer> ibo) : m_vao(vao), m_ibo(ibo), m_material(GetMaterialManager()->GetNullMaterial()) {}