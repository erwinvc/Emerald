#include "stdafx.h"

void BufferLayout::Apply(uint32 attributeIndex) {
	uint32 index = attributeIndex;
	for (BufferElement& element : m_elements) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.GetComponentCount(), VertexBufferDataTypeToOpenGLBaseType(element.m_type), element.m_normalized, m_stride, (const void*)(uint64_t)element.m_offset);
		if (element.m_divisor) glVertexAttribDivisor(index, 1);
		index++;
	}
}