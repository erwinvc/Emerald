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

	void Draw() {
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, NULL);
	}
    void Draw(uint count) {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL);
    }
};

