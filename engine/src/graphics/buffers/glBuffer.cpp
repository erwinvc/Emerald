#include "eepch.h"
#include "glBuffer.h"
#include "..\core\renderer.h"

namespace emerald {
	GLBuffer::GLBuffer(BufferTarget target, BufferUsage usage) : m_target(target), m_handle(0), m_usage(usage) {
		GL(glCreateBuffers(1, &m_handle));
		GL(glNamedBufferData(m_handle, 1, nullptr, m_usage));
	}

	GLBuffer::GLBuffer(BufferTarget target, const byte* data, uint32_t size, BufferUsage usage) : m_target(target), m_handle(0), m_usage(usage) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		GL(glCreateBuffers(1, &m_handle));
		GL(glNamedBufferData(m_handle, m_data.size(), m_data.data(), m_usage));
	}

	GLBuffer::GLBuffer(GLBuffer&& other) noexcept
		: m_handle(other.m_handle), m_data(std::move(other.m_data)), m_usage(other.m_usage), m_target(other.m_target) {
		other.m_handle = 0;
	}

	GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept {
		if (this != &other) {
			GL(glDeleteBuffers(1, &m_handle));

			m_handle = other.m_handle;
			m_data = std::move(other.m_data);
			m_usage = other.m_usage;
			m_target = other.m_target;
			other.m_handle = 0;
		}
		return *this;
	}

	GLBuffer::~GLBuffer() {
		GL(glDeleteBuffers(1, &m_handle));
	}

	void GLBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		if (m_data.size() > 0) {
			GLint bufferSize = 0;
			GL(glGetNamedBufferParameteriv(m_handle, GL_BUFFER_SIZE, &bufferSize));

			if (bufferSize < (GLint)m_data.size()) {
				GL(glNamedBufferData(m_handle, m_data.size(), m_data.data(), m_usage));
			} else {
				GL(glNamedBufferSubData(m_handle, offset, m_data.size(), m_data.data()));
			}
		}
	}

	void GLBuffer::bind() const {
		GL(glBindBuffer((uint32_t)m_target, m_handle));
	}

	VertexBuffer::VertexBuffer(BufferUsage usage) : GLBuffer(BufferTarget::ARRAY, usage) {}
	VertexBuffer::VertexBuffer(const byte* data, uint32_t size, BufferUsage usage) : GLBuffer(BufferTarget::ARRAY, data, size, usage) {}

	IndexBuffer::IndexBuffer(BufferUsage usage) : GLBuffer(BufferTarget::ELEMENTARRAY, usage) {}
	IndexBuffer::IndexBuffer(const byte* data, uint32_t size, BufferUsage usage) : GLBuffer(BufferTarget::ELEMENTARRAY, data, size, usage) {}
}