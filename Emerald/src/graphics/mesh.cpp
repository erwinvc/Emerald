#include "stdafx.h"


Mesh::Mesh(Ref<VertexArray> vao, Ref<IndexBuffer> ibo) : m_vao(vao), m_ibo(ibo), m_material(GetMaterialManager()->GetNullMaterial()) {}