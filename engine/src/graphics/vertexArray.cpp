#include "eepch.h"
#include "graphics/vertexArray.h"
#include "graphics/buffers/vertexBuffer.h"
#include "renderer.h"
#include "glError.h"

namespace emerald {
	VertexArray::VertexArray(VertexBufferLayout layout) : m_layout(layout), m_handle(0), m_validated(false) {
		Renderer::submit([instance = Ref<VertexArray>(this)]() mutable {
			GL(glGenVertexArrays(1, &instance->m_handle));

			const std::string name = "VertexArrayObject";
		//	GL(glObjectLabel(GL_VERTEX_ARRAY, instance->m_handle, static_cast<GLsizei>(label.size()), label.c_str()));
			GL(glBindVertexArray(instance->m_handle));

			GL(glObjectLabel(GL_VERTEX_ARRAY, instance->m_handle, -1, name.c_str()));
		});
	}
	VertexArray::~VertexArray() {
		auto handle = m_handle;
		Renderer::submitFromAnyThread([handle]() mutable {
			GL(glDeleteVertexArrays(1, &handle));
		});
	}

	void VertexArray::addBuffer(const Ref<VertexBuffer>& buffer) {
		for (const auto& existingBuffer : m_buffers) {
			if (buffer->handle() != existingBuffer->handle()) Log::error("VAO already contains buffer");
		}

		m_buffers.push_back(buffer);
		m_validated = false;
	}

	void VertexArray::validate() {
		Renderer::submit([instance = Ref<VertexArray>(this)]() mutable {
			GL(glBindVertexArray(instance->m_handle));

			uint32_t index = 0;
			for (const BufferElement& element : instance->m_layout.elements()) {
				ASSERT(element.m_bufferIndex < instance->m_buffers.size(), "Invalid buffer index");

				GL(glBindBuffer(GL_ARRAY_BUFFER, instance->m_buffers[element.m_bufferIndex]->handle()));
				GL(glEnableVertexAttribArray(index));

				uint32_t baseType = VertexAttribute::toBaseType(element.m_type);
				uint32_t componentCount = VertexAttribute::toComponentCount(element.m_type);

				if (VertexAttribute::isIntegerType(element.m_type)) {
					GL(glVertexAttribIPointer(index, componentCount, baseType, instance->m_layout.stride(), (const void*)(uint64_t)element.m_offset));
				} else {
					GL(glVertexAttribPointer(index, componentCount, baseType, element.m_normalized, instance->m_layout.stride(), (const void*)(uint64_t)element.m_offset));
				}
				if (element.m_divisor) {
					GL(glVertexAttribDivisor(index, 1));
				}
				index++;
			}
			GL(glBindVertexArray(0));
			instance->m_validated = true;
		});
	}

	void VertexArray::bind() const {
		Renderer::submit([instance = Ref<const VertexArray>(this)] {
			ASSERT(instance->m_validated, "VertexArray not validated");
			GL(glBindVertexArray(instance->handle()));
		});
	}
}