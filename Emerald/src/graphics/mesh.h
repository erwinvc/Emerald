#pragma once

class Mesh {
private:
	Ref<VertexArray> m_vao;
	Ref<IndexBuffer> m_ibo;
	Ref<Material> m_material;

public:
	Mesh() : m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
	Mesh(Ref<VertexArray> vao, Ref<IndexBuffer> ibo, Ref<Material> mat) : m_vao(vao), m_ibo(ibo), m_material(mat) {}
	Mesh(Ref<VertexArray> vao, Ref<IndexBuffer> ibo);
	~Mesh() {}
	Material* GetMaterial() { return m_material; }
	void SetMaterial(Ref<Material> mat) { m_material = mat; }

	void Bind() {
		m_vao->Bind();
		m_ibo->Bind();
	}

	void Unbind() {
		m_vao->Unbind();
		m_ibo->Unbind();
	}

	void DrawInstanced(int amount, int mode = GL_TRIANGLES) {
		Bind();
		m_ibo->DrawInstanced(amount, mode);
		//Unbind();
	}

	void Draw() {
		Bind();
		m_ibo->Draw();
		//Unbind();
	}

	inline Ref<VertexArray> GetVAO() { return m_vao; }
	inline Ref<IndexBuffer> GetIBO() { return m_ibo; }
};
