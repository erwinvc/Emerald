#pragma once
class IndexBuffer {
private:
	GLuint m_bufferID;
	GLuint m_count;
public:
	IndexBuffer(GLushort* data, GLsizei count);
	~IndexBuffer();
	inline GLuint GetCount() { return m_count; }

	void Bind();
	void Unbind();

};

