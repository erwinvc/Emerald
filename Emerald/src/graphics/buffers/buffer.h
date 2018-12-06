#pragma once
class Buffer {
private:
	GLuint m_bufferID;
	GLuint m_componentCount;
public:
	Buffer(GLfloat* data, GLsizei count, GLuint componentCount);
	inline GLuint GetComponentCount() { return m_componentCount; }

	void Bind();
	void Unbind();

};

