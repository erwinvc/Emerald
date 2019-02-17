#pragma once
class Buffer {
private:
    GLuint m_bufferID;
    GLuint m_componentCount;
    GLfloat* m_data;
public:
    Buffer(GLfloat* data, GLuint count, GLuint componentCount, GLenum usage = GL_DYNAMIC_DRAW);
    ~Buffer();

    inline GLuint GetComponentCount() { return m_componentCount; }

    inline void Bind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
    }
    inline void Unbind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};

