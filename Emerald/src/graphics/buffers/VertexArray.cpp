#include "stdafx.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_arrayID);
}
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_arrayID);
	for (int i = 0; i < m_buffers.size(); i++) {
		delete m_buffers[i];
	}
}

void VertexArray::AddBuffer(Buffer* buffer, GLuint index, bool divisor) {
	Bind();
	buffer->Bind();
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, buffer->GetComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);
	if (divisor) glVertexAttribDivisor(index, 1);

	buffer->Unbind();
	Unbind();

	m_buffers.push_back(buffer);

}
void VertexArray::Bind() {
	glBindVertexArray(m_arrayID);
	for (auto& buffer : m_buffers) {
		buffer->Bind();
	}
}
void VertexArray::Unbind() {
	glBindVertexArray(0);
	for (auto& buffer : m_buffers) {
		buffer->Unbind();
	}
}


