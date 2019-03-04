#pragma once
class Buffer {
private:
    uint m_bufferID;
    uint m_componentCount;
    GLfloat* m_data;
public:
    Buffer(GLfloat* data, uint count, uint componentCount, GLenum usage = GL_DYNAMIC_DRAW);
    ~Buffer();

    inline uint GetComponentCount() { return m_componentCount; }

    inline uint GetID() { return m_bufferID; }

    inline void Bind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
    }
    inline void Unbind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};

