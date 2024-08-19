#include "eepch.h"
#include "vertexBuffer.h"
#include "util/buffer.h"
#include "../renderer.h"
#include "glError.h"

namespace emerald {
	VertexBuffer::VertexBuffer(const byte* data, uint32_t size, BufferUsage usage) : m_handle(0), m_usage(usage) {
		m_data = Buffer<byte>::copy((byte*)data, size);
		Ref<VertexBuffer> instance = this;
		Renderer::submit([instance]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glBindBuffer(GL_ARRAY_BUFFER, instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), instance->m_usage));
		});
	}

	VertexBuffer::~VertexBuffer() {
		auto handle = m_handle;
		Renderer::submitFromAnyThread([handle]() mutable {
			GL(glDeleteBuffers(1, &handle));
		});
	}

	void VertexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Ref<VertexBuffer> instance = this;
		Renderer::submit([instance, offset]() mutable {
			GL(glBindBuffer(GL_ARRAY_BUFFER, instance->m_handle));
			GL(glNamedBufferSubData(instance->m_handle, offset, instance->m_data.size(), instance->m_data.data()));
		});
	}

	void VertexBuffer::bind() const {
		auto handle = m_handle;
		Renderer::submit([handle]() mutable {
			GL(glBindBuffer(GL_ARRAY_BUFFER, handle));
		});
	}
}