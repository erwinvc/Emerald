#pragma once
class IndexBuffer {
private:
    GLuint m_bufferID;
    GLuint m_count;
public:
    IndexBuffer(uint* data, uint32 count);
    ~IndexBuffer();
    inline GLuint GetCount() { return m_count; }

    void Bind();
    void Unbind();

	void SetData(uint* data, uint32 count, GLenum usage = GL_STATIC_DRAW) {
		m_count = count;
		Bind();
		GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, usage));
		Unbind();
	}

	void Draw(uint mode) {
		GL(glDrawElements(mode, m_count, GL_UNSIGNED_INT, NULL));
	}
    void Draw(uint32 count, uint mode) {
        GL(glDrawElements(mode, count, GL_UNSIGNED_INT, NULL));
    }
    void DrawInstanced(uint32 amount, uint mode = GL_TRIANGLES) {
        GL(glDrawElementsInstanced(mode, m_count, GL_UNSIGNED_INT, 0, amount));
    }
};

