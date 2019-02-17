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
    ~Mesh() {
        delete m_vao;
        delete m_ibo;
    }
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

    void DrawInstanced(int amount) {
        Bind();
        m_ibo->DrawInstanced(amount);
        //Unbind();
    }

    void Draw() {
        Bind();
        m_ibo->Draw();
        //Unbind();
    }

    inline VertexArray* GetVAO() { return m_vao; }
    inline IndexBuffer* GetIBO() { return m_ibo; }
};
