#include "stdafx.h"


IndexBuffer::IndexBuffer(uint* data, GLuint count) : m_count(count) {
	GL(glGenBuffers(1, &m_bufferID));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_bufferID);
}

void IndexBuffer::Bind() {
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID));
}
void IndexBuffer::Unbind() {
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}