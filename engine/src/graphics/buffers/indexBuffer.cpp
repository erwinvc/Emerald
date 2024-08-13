#include "eepch.h"
#include "indexBuffer.h"
#include "../renderer.h"

namespace emerald {
	IndexBuffer::IndexBuffer(const byte* data, uint32_t size) {
		glCreateBuffers(1, &m_handle);
		glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
		m_data = Buffer<byte>::copy((byte*)data, size);
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_handle);
	}

	void IndexBuffer::setData(const byte* data, uint32_t size, uint32_t offset) {
		m_data = Buffer<byte>::copy((byte*)data, size);

		//Ref<IndexBuffer> instance = this;
		//Renderer::submit([instance, offset, size]() {
			glNamedBufferSubData(m_handle, offset, size, m_data.data());
		//});
	}

	void IndexBuffer::bind() const {
		//Renderer::submit([=]() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		//});
	}
}