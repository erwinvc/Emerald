#pragma once
class IndexBuffer {
private:
    GLuint m_bufferID;
    GLuint m_count;
public:
    IndexBuffer(uint* data, GLuint count);
    ~IndexBuffer();
    inline GLuint GetCount() { return m_count; }

    void Bind();
    void Unbind();

	void Draw(uint mode = GL_TRIANGLES) {
		GL(glDrawElements(mode, m_count, GL_UNSIGNED_INT, NULL));
	}
    void Draw(uint32 count, uint mode = GL_TRIANGLES) {
        GL(glDrawElements(mode, count, GL_UNSIGNED_INT, NULL));
    }
    void DrawInstanced(uint32 amount, uint mode = GL_TRIANGLES) {
        GL(glDrawElementsInstanced(mode, m_count, GL_UNSIGNED_INT, 0, amount));
    }
};

