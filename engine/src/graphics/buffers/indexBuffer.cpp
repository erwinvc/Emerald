#include "eepch.h"
#include "indexBuffer.h"
#include "graphics/core/renderer.h"

namespace emerald {
	IndexBuffer::IndexBuffer(const byte* data, uint32_t size) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<IndexBuffer>(this)]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), GL_STATIC_DRAW));
		});
	}

	IndexBuffer::~IndexBuffer() {
		Renderer::submitFromAnyThread([handle = m_handle]() mutable {
			GL(glDeleteBuffers(1, &handle));
		});
	}

	void IndexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Renderer::submit([instance = Ref<IndexBuffer>(this), offset]() mutable {
			GL(glNamedBufferSubData(instance->m_handle, offset, instance->m_data.size(), instance->m_data.data()));
		});
	}

	void IndexBuffer::bind() const {
		Renderer::submit([handle = m_handle]() mutable {
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle));
		});
	}
}