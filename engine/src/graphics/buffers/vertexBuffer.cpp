#include "eepch.h"
#include "vertexBuffer.h"
#include "util/buffer.h"

namespace emerald {
	VertexBuffer::VertexBuffer(const byte* data, uint32_t size, BufferUsage usage) : m_usage(usage) {
		glCreateBuffers(1, &m_handle);
		glNamedBufferData(m_handle, size, data, m_usage);
		m_data = Buffer<byte>::copy((byte*)data, size);
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_handle);
	}

	void VertexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);
		glNamedBufferSubData(m_handle, offset, size, data);
	}

	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);
	}
}