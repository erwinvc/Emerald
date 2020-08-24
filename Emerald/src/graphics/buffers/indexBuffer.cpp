#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

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

void IndexBuffer::SetData(uint* data, uint32 count, GLenum usage) {
	m_count = count;
	Bind();
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, usage));
	Unbind();
}

void IndexBuffer::Draw(uint mode) {
	if (m_count > 0)
		GL(glDrawElements(mode, m_count, GL_UNSIGNED_INT, NULL));
}
void IndexBuffer::Draw(uint32 count, uint mode) {
	if (count > 0)
		GL(glDrawElements(mode, count, GL_UNSIGNED_INT, NULL));
}
void IndexBuffer::DrawInstanced(uint32 amount, uint mode) {
	if (m_count > 0)
	GL(glDrawElementsInstanced(mode, m_count, GL_UNSIGNED_INT, 0, amount));
}