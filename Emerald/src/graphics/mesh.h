#pragma once

class Mesh {
protected:
	ManagedRef<VertexArray> m_vao;
	ManagedRef<IndexBuffer> m_ibo;
	AssetRef<Material> m_material;

public:
	Mesh();
	Mesh(ManagedRef<VertexArray>& vao, ManagedRef<IndexBuffer>& ibo, Material* mat) : m_vao(vao), m_ibo(ibo), m_material(mat) {}
	Mesh(ManagedRef<VertexArray>& vao, ManagedRef<IndexBuffer>& ibo);
	~Mesh() {}
	AssetRef<Material> GetMaterial() { return m_material; }
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

	void Draw(uint32 count, uint mode = GL_TRIANGLES) {
		Bind();
		m_ibo->Draw(count, mode);
		//Unbind();
	}

	inline ManagedRef<VertexArray> GetVAO() { return m_vao; }
	inline ManagedRef<IndexBuffer> GetIBO() { return m_ibo; }
};
