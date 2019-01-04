#pragma once
class Buffer {
private:
	GLuint m_bufferID;
    GLuint m_componentCount;
public:
	Buffer(GLfloat* data, GLuint count, GLuint componentCount, GLenum usage = GL_STATIC_DRAW);
	~Buffer();
	inline GLuint GetComponentCount() { return m_componentCount; }

	void Bind();
	void Unbind();

};

