#include "eepch.h"
#include "indexBuffer.h"
#include "../renderer.h"
#include "glError.h"

namespace emerald {
	IndexBuffer::IndexBuffer(const byte* data, uint32_t size) {
		m_data = Buffer<byte>::copy((byte*)data, size);
		Ref<IndexBuffer> instance = this;
		Renderer::submit([instance]() mutable {
			GL(glCreateBuffers(1, &instance->m_handle));
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_handle));
			GL(glNamedBufferData(instance->m_handle, instance->m_data.size(), instance->m_data.data(), GL_STATIC_DRAW));
		});
	}

	IndexBuffer::~IndexBuffer() {
		auto handle = m_handle;
		Renderer::submitFromAnyThread([handle]() mutable {
			GL(glDeleteBuffers(1, &handle));
		});
	}

	void IndexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		Ref<IndexBuffer> instance = this;
		Renderer::submit([instance, offset]() mutable {
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_handle));
			GL(glNamedBufferSubData(instance->m_handle, offset, instance->m_data.size(), instance->m_data.data()));
		});
	}

	void IndexBuffer::bind() const {
		auto handle = m_handle;
		Renderer::submit([handle]() mutable {
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle));
		});
	}
}