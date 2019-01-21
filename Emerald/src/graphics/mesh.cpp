#include "stdafx.h"


Mesh::Mesh(VertexArray* vao, IndexBuffer* ibo) : m_vao(vao), m_ibo(ibo), m_material(GetMaterialManager()->GetNullMaterial()) {}