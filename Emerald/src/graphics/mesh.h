#pragma once

class IndexBuffer;
class Mesh : public AssetBase {
protected:
	ManagedRef<VertexArray> m_vao;
	ManagedRef<IndexBuffer> m_ibo;
	AssetRef<Material> m_material;

public:
	glm::vec3 m_size;
	glm::vec3 m_center;

	Mesh();
	Mesh(ManagedRef<VertexArray> vao, ManagedRef<IndexBuffer> ibo, Material* material) : m_vao(vao), m_ibo(ibo) {
		m_material = material;
	}
	Mesh(ManagedRef<VertexArray> vao, ManagedRef<IndexBuffer> ibo);
	virtual ~Mesh() {}
	Material* GetMaterial() { return m_material; }
	void SetMaterial(Material* mat) { m_material = mat; }

	void Bind();
	void Unbind();
	
	void DrawInstanced(int amount, uint mode = GL_TRIANGLES);
	void Draw(uint mode = GL_TRIANGLES);
	void DrawCount(uint32 count, uint mode = GL_TRIANGLES);
	void DrawArrays(uint32 count, uint mode = GL_TRIANGLES);
	
	AssetRef<Mesh> Copy();

	inline ManagedRef<VertexArray> GetVAO() { return m_vao; }
	inline ManagedRef<IndexBuffer> GetIBO() { return m_ibo; }

	static const String GetAssetTypeName() { return "Mesh"; }
};
