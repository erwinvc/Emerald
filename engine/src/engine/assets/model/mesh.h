#pragma once
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/glBuffer.h"
#include "graphics/render/material.h"

namespace emerald {
	class Mesh : public RefCounted {
	protected:
		std::string m_name;
		Ref<VertexArray> m_vao;
		Ref<IndexBuffer> m_ibo;
		Ref<Material> m_material;

	public:
		Mesh() : m_name("Mesh"), m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
		Mesh(std::string name, const Ref<VertexArray>& vao, const Ref<IndexBuffer>& ibo) : m_name(name), m_vao(vao), m_ibo(ibo), m_material(nullptr) {}
		Mesh(std::string name, const Ref<VertexArray>& vao, const Ref<IndexBuffer>& ibo, Ref<Material> mat) : m_name(name), m_vao(vao), m_ibo(ibo), m_material(mat) {}

		~Mesh() {}
		const Ref<Material>& getMaterial() { return m_material; }
		void setMaterial(const Ref<Material>& mat) { m_material = mat; }
		const std::string& getName() const { return m_name; }

		void bind() {
			m_vao->bind();
			m_ibo->bind();
		}

		const Ref<VertexArray>& getVAO() const { return m_vao; }
		const Ref<IndexBuffer>& getIBO() const { return m_ibo; }
	};
}