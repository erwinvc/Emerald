#pragma once
class IndexBuffer {
private:
	GLuint m_bufferID;
	GLuint m_count;
public:
	IndexBuffer(uint* data, uint count);
	~IndexBuffer();
	inline GLuint GetCount() { return m_count; }

	void Bind();
	void Unbind();

	void Draw() {
		//glDrawElementsInstanced(GL_TRIANGLES, m_count, GL_UNSIGNED_SHORT, 0, 1);
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, NULL);
	}
};

