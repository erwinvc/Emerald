#pragma once
#include "graphics/vertexArray.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/material.h"

namespace emerald {
	class Mesh : public RefCounted {
	protected:
		Ref<VertexArray> m_vao;
		Ref<IndexBuffer> m_ibo;
		Ref<Material> m_material;

	public:
		Mesh() : m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
		Mesh(Ref<VertexArray>& vao, Ref<IndexBuffer>& ibo) : m_vao(vao), m_ibo(ibo), m_material(nullptr) {}
		Mesh(Ref<VertexArray>& vao, Ref<IndexBuffer>& ibo, Ref<Material> mat) : m_vao(vao), m_ibo(ibo), m_material(mat) {}

		virtual ~Mesh() {}
		Ref<Material> getMaterial() { return m_material; }
		void setMaterial(Ref<Material> mat) { m_material = mat; }

		void bind() {
			m_vao->bind();
			m_ibo->bind();
		}


		//void DrawInstanced(int amount, uint mode = GL_TRIANGLES) {
		//	bind();
		//	m_ibo->DrawInstanced(amount, mode);
		//	//Unbind();
		//}
		//
		//void Draw(uint mode = GL_TRIANGLES) {
		//	Bind();
		//	m_ibo->Draw(mode);
		//	//Unbind();
		//}
		//
		//void DrawCount(uint32 count, uint mode = GL_TRIANGLES) {
		//	Bind();
		//	m_ibo->Draw(count, mode);
		//	//Unbind();
		//}
		//
		//AssetRef<Mesh> Copy() {
		//	return NEW(Mesh(m_vao, m_ibo, m_material));
		//}

		Ref<VertexArray> getVAO() const { return m_vao; }
		Ref<IndexBuffer> getIBO() const { return m_ibo; }
	};
}