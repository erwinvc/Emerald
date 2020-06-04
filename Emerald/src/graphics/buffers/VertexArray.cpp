#include "stdafx.h"

VertexArray::VertexArray() {
    GL(glGenVertexArrays(1, &m_arrayID));
}
VertexArray::~VertexArray() {
    GL(glDeleteVertexArrays(1, &m_arrayID));
}

void VertexArray::AddBuffer(ManagedRef<VertexBuffer> buffer) {
    for (auto buff : m_buffers) {
        ASSERT(buffer->GetHandle() != buff->GetHandle(), "VAO already contains this buffer");
    }
    m_buffers.push_back(buffer);
}

void VertexArray::ApplyLayouts() {
	Bind();
	uint32 attributeIndex = 0;
	for (int i = 0; i < m_buffers.size(); i++) {
		m_buffers[i]->ApplyLayout(attributeIndex);
		attributeIndex += m_buffers[i]->GetLayout().GetElements().size();
	}
	Unbind();
}

void VertexArray::Bind() {
    GL(glBindVertexArray(m_arrayID));
	uint32 attributeIndex = 0;
	for (int i = 0; i < m_buffers.size(); i++) {
		m_buffers[i]->ApplyLayout(attributeIndex);
		attributeIndex += m_buffers[i]->GetLayout().GetElements().size();
	}
}

void VertexArray::Unbind() {
    GL(glBindVertexArray(0));
}


