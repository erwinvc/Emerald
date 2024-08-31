#include "eepch.h"
#include "vertexBuffer.h"
#include "util/buffer.h"
#include "../renderer.h"
#include "glError.h"

namespace emerald {
	VertexBuffer::VertexBuffer(const byte* data, uint32_t size, BufferUsage usage) : m_handle(0), m_usage(usage) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<VertexBuffer>(this)]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), instance->m_usage));
		});
	}

	VertexBuffer::~VertexBuffer() {
		Renderer::submitFromAnyThread([handle = m_handle]() mutable {
			GL(glDeleteBuffers(1, &handle));
		});
	}

	void VertexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<VertexBuffer>(this), offset]() mutable {
			GL(glNamedBufferSubData(instance->m_handle, offset, instance->m_data.size(), instance->m_data.data()));
		});
	}

	void VertexBuffer::bind() const {
		Renderer::submit([handle = m_handle]() mutable {
			GL(glBindBuffer(GL_ARRAY_BUFFER, handle));
		});
	}
}