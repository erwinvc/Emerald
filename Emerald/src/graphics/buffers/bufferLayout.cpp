#include "stdafx.h"

BufferLayout::BufferLayout(const initializer_list<BufferElement>& elements) : m_elements(elements) {
	CalculateOffsetsAndStride();
}

uint32 BufferElement::GetComponentCount() const {
	switch (m_type) {
		case VertexBufferDataType::Bool: return 1;
		case VertexBufferDataType::Float: return 1;
		case VertexBufferDataType::Float2: return 2;
		case VertexBufferDataType::Float3: return 3;
		case VertexBufferDataType::Float4: return 4;
		case VertexBufferDataType::UInt: return 1;
		case VertexBufferDataType::Int: return 1;
		case VertexBufferDataType::Int2: return 2;
		case VertexBufferDataType::Int3: return 3;
		case VertexBufferDataType::Int4: return 4;
		case VertexBufferDataType::Mat3: return 9;
		case VertexBufferDataType::Mat4: return 16;
	}
	LOG_ERROR("[~cBuffers~x] Unknow VertexBufferDataType!");
	return 0;
}

uint32 BufferLayout::GetTotalComponentCountSize() {
	uint32 count = 0;
	for (BufferElement& element : m_elements) count += element.GetComponentCount() * sizeof(VertexBufferDataTypeToOpenGLBaseType(element.m_type));
	return count;
}

void BufferLayout::Apply(uint32 attributeIndex) {
	uint32 index = attributeIndex;
	for (BufferElement& element : m_elements) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.GetComponentCount(), VertexBufferDataTypeToOpenGLBaseType(element.m_type), element.m_normalized, m_stride, (const void*)(uint64_t)element.m_offset);
		if (element.m_divisor) glVertexAttribDivisor(index, 1);
		index++;
	}
}