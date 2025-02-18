#include "eepch.h"
#include "glBuffer.h"
#include "..\core\renderer.h"

namespace emerald {
	GLBuffer::GLBuffer(BufferTarget target, BufferUsage usage) : m_target(target), m_handle(0), m_usage(usage) {
		Renderer::submit([instance = Ref<GLBuffer>(this)]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, 1, nullptr, instance->m_usage));
		});
	}

	GLBuffer::GLBuffer(BufferTarget target, const byte* data, uint32_t size, BufferUsage usage) : m_target(target), m_handle(0), m_usage(usage) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<GLBuffer>(this)]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), instance->m_usage));
		});
	}

	GLBuffer::GLBuffer(GLBuffer&& other) noexcept
		: m_handle(other.m_handle), m_data(std::move(other.m_data)), m_usage(other.m_usage), m_target(other.m_target) {
		other.m_handle = 0;  
	}

	GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept {
		if (this != &other) {
			Renderer::submitFromAnyThread([handle = m_handle]() mutable {
				GL(glDeleteBuffers(1, &handle));
			});

			m_handle = other.m_handle;
			m_data = std::move(other.m_data);
			m_usage = other.m_usage;
			m_target = other.m_target;
			other.m_handle = 0;
		}
		return *this;
	}

	GLBuffer::~GLBuffer() {
		Renderer::submitFromAnyThread([handle = m_handle]() mutable {
			GL(glDeleteBuffers(1, &handle));
		});
	}

	void GLBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<GLBuffer>(this), offset]() mutable {
			if (instance->m_data.size() > 0) {
				GLint bufferSize = 0;
				GL(glGetNamedBufferParameteriv(instance->m_handle, GL_BUFFER_SIZE, &bufferSize));

				if (bufferSize < (GLint)instance->m_data.size()) {
					GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), instance->m_usage));
				} else {
					GL(glNamedBufferSubData(instance->m_handle, offset, instance->m_data.size(), instance->m_data.data()));
				}
			}
		});
	}

	void GLBuffer::bind() const {
		Renderer::submit([instance = Ref<const GLBuffer>(this)]() mutable {
			GL(glBindBuffer((uint32_t)instance->m_target, instance->m_handle));
		});
	}

	VertexBuffer::VertexBuffer(BufferUsage usage) : GLBuffer(BufferTarget::ARRAY, usage) {}
	VertexBuffer::VertexBuffer(const byte* data, uint32_t size, BufferUsage usage) : GLBuffer(BufferTarget::ARRAY, data, size, usage) {}

	IndexBuffer::IndexBuffer(BufferUsage usage) : GLBuffer(BufferTarget::ELEMENTARRAY, usage) {}
	IndexBuffer::IndexBuffer(const byte* data, uint32_t size, BufferUsage usage) : GLBuffer(BufferTarget::ELEMENTARRAY, data, size, usage) {}
}