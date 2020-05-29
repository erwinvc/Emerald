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

void VertexArray::Bind() {
    GL(glBindVertexArray(m_arrayID));
}

void VertexArray::Unbind() {
    GL(glBindVertexArray(0));
}


