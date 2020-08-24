#include "stdafx.h"

VertexBuffer::VertexBuffer(const void* data, uint32 vertexCount, BufferLayout layout, GLenum usage) : m_layout(layout), m_data(data) {
	GL(glGenBuffers(1, &m_bufferID));
	SetData(data, vertexCount, usage);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_bufferID);
}

void VertexBuffer::ApplyLayout(uint32 attributeIndex) {
	Bind();
	m_layout.Apply(attributeIndex);
	Unbind();
}

void VertexBuffer::SetData(const void* data, uint32 vertexCount, GLenum usage) {
	Bind();
	GL(glBufferData(GL_ARRAY_BUFFER, vertexCount * m_layout.GetTotalComponentCountSize(), data, usage));
	Unbind();
}

