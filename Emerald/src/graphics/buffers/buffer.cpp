#include "stdafx.h"

Buffer::Buffer(GLfloat* data, GLuint count, GLuint componentCount, GLenum usage) : m_componentCount(componentCount) {
	glGenBuffers(1, &m_bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &m_bufferID);
}

void Buffer::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
}
void Buffer::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


