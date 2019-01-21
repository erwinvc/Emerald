#pragma once

class Mesh {
private:
	VertexArray* m_vao;
	IndexBuffer* m_ibo;
    Material* m_material;

public:
    Mesh() : m_vao(nullptr), m_ibo(nullptr), m_material(nullptr) {}
    Mesh(VertexArray* vao, IndexBuffer* ibo, Material* mat) : m_vao(vao), m_ibo(ibo), m_material(mat) {}
    Mesh(VertexArray* vao, IndexBuffer* ibo);

    Material* GetMaterial() { return m_material;}
    void SetMaterial(Material* mat) {m_material = mat; }

	void Draw() {
		m_vao->Bind();
		m_ibo->Bind();

		m_ibo->Draw();
	}
};
