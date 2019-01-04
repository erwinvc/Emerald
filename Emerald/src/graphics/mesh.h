#pragma once

class Mesh {
private:
	VertexArray* m_vao;
	IndexBuffer* m_ibo;

public:
    Mesh() : m_vao(nullptr), m_ibo(nullptr) {}
	Mesh(VertexArray* vao, IndexBuffer* ibo) : m_vao(vao), m_ibo(ibo) {}

	void Draw() {
		m_vao->Bind();
		m_ibo->Bind();

		m_ibo->Draw();
	}
};