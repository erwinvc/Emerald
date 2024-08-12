#include "eepch.h"
#include "graphics/vertexArray.h"
#include "graphics/buffers/vertexBuffer.h"

namespace emerald {
	VertexArray::VertexArray(VertexBufferLayout layout) : m_layout(layout), m_validated(false) {
		glGenVertexArrays(1, &m_handle);
	}
	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_handle);
	}

	void VertexArray::addBuffer(std::shared_ptr<VertexBuffer> buffer) {
		for (const auto& existingBuffer : m_buffers) {
			if (buffer->handle() != existingBuffer->handle()) Log::error("VAO already contains buffer");
		}

		m_buffers.push_back(buffer);
		m_validated = false;
	}

    void VertexArray::validate() {
		glBindVertexArray(m_handle);

        uint32_t index = 0;
        for (const BufferElement& element : m_layout.elements()) {
            ASSERT(element.m_bufferIndex < m_buffers.size(), "Invalid buffer index");

            m_buffers[element.m_bufferIndex]->bind();
            glEnableVertexAttribArray(index);

            uint32_t baseType = VertexAttribute::toBaseType(element.m_type);
            uint32_t componentCount = VertexAttribute::toComponentCount(element.m_type);

            if (VertexAttribute::isIntegerType(element.m_type)) {
                glVertexAttribIPointer(index, componentCount, baseType, m_layout.stride(), (const void*)(uint64_t)element.m_offset);
            } else {
                glVertexAttribPointer(index, componentCount, baseType, element.m_normalized, m_layout.stride(), (const void*)(uint64_t)element.m_offset);
            }
            if (element.m_divisor) {
                glVertexAttribDivisor(index, 1);
            }

            index++;
        }
        m_validated = true;
    }

	void VertexArray::bind() const {
		ASSERT(m_validated, "VertexArray not validated");
		glBindVertexArray(m_handle);
	}
}