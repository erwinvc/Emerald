#pragma once
class Buffer {
private:
	uint32 m_bufferID;
	uint32 m_componentCount;
	//BufferLayout m_layout;
    GLfloat* m_data;
public:
    Buffer(GLfloat* data, uint32 count, uint32 componentCount, GLenum usage = GL_STATIC_DRAW);
    ~Buffer();

	//inline const BufferLayout& GetLayout() { return m_layout; }
	//inline void SetLayout(BufferLayout& layout) { m_layout = layout; }

    inline uint32 GetComponentCount() { return m_componentCount; }

    inline uint32 GetID() { return m_bufferID; }

    inline void Bind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
    }
    inline void Unbind() {
        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};

