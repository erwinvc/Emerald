#include "stdafx.h"


IndexBuffer::IndexBuffer(uint* data, uint32 count) : m_count(count) {
	GL(glGenBuffers(1, &m_bufferID));
	SetData(data, count);
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