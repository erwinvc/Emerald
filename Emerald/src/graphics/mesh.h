#pragma once

class Mesh {
private:
	shared_ptr<VertexArray> m_vao;
	shared_ptr<IndexBuffer> m_ibo;
	Material* m_material;

public:
	Mesh() : m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
	Mesh(shared_ptr<VertexArray> vao, shared_ptr<IndexBuffer> ibo, Material* mat) : m_vao(vao), m_ibo(ibo), m_material(mat) {}
	Mesh(shared_ptr<VertexArray> vao, shared_ptr<IndexBuffer> ibo);
	~Mesh() {}
	Material* GetMaterial() { return m_material; }
	void SetMaterial(Material* mat) { m_material = mat; }

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

	inline shared_ptr<VertexArray> GetVAO() { return m_vao; }
	inline shared_ptr<IndexBuffer> GetIBO() { return m_ibo; }
};
