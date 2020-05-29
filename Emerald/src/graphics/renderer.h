#pragma once

class IndexBuffer;
class Renderer {
private:
    bool m_started;
    bool m_ended;
    const int MAX_OBJECTS = 1000000;
	ManagedRef<VertexArray> m_vao;
	ManagedRef<IndexBuffer> m_ibo;
	ManagedRef<VertexBuffer> m_offsets;
    int m_amount;
    int m_numindices;
    glm::vec3* m_offsetsTempBuffer;
    glm::vec3* m_offsetsPtr;
public:
    void Begin();
	void Submit(glm::vec3& offset);
    void End();
    void Draw();

    Renderer(int numvertices, int numindices, float* vertices, float* normals, float* texcoords, float* tangents, uint* data);
	~Renderer();
};

