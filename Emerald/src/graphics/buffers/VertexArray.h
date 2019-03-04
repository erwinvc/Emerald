#pragma once


class VertexArray {
private:
    GLuint m_arrayID;
    vector<Buffer*> m_buffers;
public:
    VertexArray();
    ~VertexArray();

    int GetBufferCount() { return m_buffers.size(); }
    vector<Buffer*> GetBuffers() { return m_buffers; }
    Buffer* GetBuffer(int index) { return m_buffers.at(index); }
    void AddBuffer(Buffer* buffer, GLuint index, bool divisor);
    void Bind();
    void Unbind();
};