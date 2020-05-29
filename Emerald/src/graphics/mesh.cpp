#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

Mesh::Mesh() : m_vao(nullptr), m_ibo(nullptr) {
	m_material = GetMaterialManager()->GetBasicMaterial(GetShaderManager()->Get("Geometry"));
}
Mesh::Mesh(ManagedRef<VertexArray> vao, ManagedRef<IndexBuffer> ibo) : m_vao(vao), m_ibo(ibo) {
	m_material = GetMaterialManager()->GetBasicMaterial(GetShaderManager()->Get("Geometry"));
}

void Mesh::Bind() {
	m_vao->Bind();
	m_ibo->Bind();
}

void Mesh::Unbind() {
	m_vao->Unbind();
	m_ibo->Unbind();
}

void Mesh::DrawInstanced(int amount, uint mode) {
	Bind();
	m_ibo->DrawInstanced(amount, mode);
}

void Mesh::Draw(uint mode) {
	Bind();
	//GL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	m_ibo->Draw(mode);
}

void Mesh::DrawCount(uint32 count, uint mode) {
	Bind();
	GL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	m_ibo->Draw(count, mode);
}

void Mesh::DrawArrays(uint32 count, uint mode) {
	Bind();
	glDrawArrays(mode, 0, count);
}

AssetRef<Mesh> Mesh::Copy() {
	return NEW(Mesh(m_vao, m_ibo, m_material));
}