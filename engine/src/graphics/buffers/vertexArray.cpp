#include "eepch.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/core/renderer.h"

namespace emerald {
	VertexArray::VertexArray(const std::string& name, VertexBufferLayout layout) : m_layout(layout), m_handle(0), m_validated(false) {
		GL(glGenVertexArrays(1, &m_handle));

		GL(glBindVertexArray(m_handle));
		GL(glObjectLabel(GL_VERTEX_ARRAY, m_handle, -1, name.c_str()));
	}

	VertexArray::VertexArray(VertexBufferLayout layout) : m_layout(layout), m_handle(0), m_validated(false) {
		GL(glGenVertexArrays(1, &m_handle));

		const std::string name = "VertexArrayObject";
		GL(glBindVertexArray(m_handle));
		GL(glObjectLabel(GL_VERTEX_ARRAY, m_handle, -1, name.c_str()));
	}

	VertexArray::~VertexArray() {
		GL(glDeleteVertexArrays(1, &m_handle));
	}

	void VertexArray::addBuffer(const Ref<VertexBuffer>& buffer) {
		for (const auto& existingBuffer : m_buffers) {
			if (buffer->handle() != existingBuffer->handle()) Log::error("VAO already contains buffer");
		}

		m_buffers.push_back(buffer);
		m_validated = false;
	}

	void VertexArray::validate() {
		GL(glBindVertexArray(m_handle));

		uint32_t index = 0;
		for (const BufferElement& element : m_layout.elements()) {
			ASSERT(element.m_bufferIndex < m_buffers.size(), "Invalid buffer index");

			GL(glBindBuffer(GL_ARRAY_BUFFER, m_buffers[element.m_bufferIndex]->handle()));
			GL(glEnableVertexAttribArray(index));

			uint32_t baseType = VertexAttribute::toBaseType(element.m_type);
			uint32_t componentCount = VertexAttribute::toComponentCount(element.m_type);

			if (VertexAttribute::isIntegerType(element.m_type)) {
				GL(glVertexAttribIPointer(index, componentCount, baseType, m_layout.stride(), (const void*)(uint64_t)element.m_offset));
			} else {
				GL(glVertexAttribPointer(index, componentCount, baseType, element.m_normalized, m_layout.stride(), (const void*)(uint64_t)element.m_offset));
			}
			if (element.m_divisor) {
				GL(glVertexAttribDivisor(index, 1));
			}
			index++;
		}
		GL(glBindVertexArray(0));
		m_validated = true;
	}

	void VertexArray::bind() const {
		ASSERT(m_validated, "VertexArray not validated");
		GL(glBindVertexArray(handle()));
	}
}