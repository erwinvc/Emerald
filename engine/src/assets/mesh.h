#pragma once
#include "graphics/vertexArray.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/material.h"

namespace emerald {
	class Mesh : public RefCounted {
	protected:
		std::string m_name;
		Ref<VertexArray> m_vao;
		Ref<IndexBuffer> m_ibo;
		Ref<Material> m_material;

	public:
		Mesh() : m_name("Mesh"), m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
		Mesh(std::string name, Ref<VertexArray>& vao, Ref<IndexBuffer>& ibo) : m_name(name), m_vao(vao), m_ibo(ibo), m_material(nullptr) {}
		Mesh(std::string name, Ref<VertexArray>& vao, Ref<IndexBuffer>& ibo, Ref<Material> mat) : m_name(name), m_vao(vao), m_ibo(ibo), m_material(mat) {}

		~Mesh() {}
		Ref<Material> getMaterial() { return m_material; }
		void setMaterial(Ref<Material> mat) { m_material = mat; }
		const std::string& getName() const { return m_name; }

		void bind() {
			m_vao->bind();
			m_ibo->bind();
		}

		Ref<VertexArray> getVAO() const { return m_vao; }
		Ref<IndexBuffer> getIBO() const { return m_ibo; }
	};
}