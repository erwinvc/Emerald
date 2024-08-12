#include "eepch.h"
#include "vertexBufferLayout.h"

namespace emerald {
	namespace VertexAttribute {
		uint32_t toSize(VertexAttributeType type) {
			switch (type) {
				case VertexAttributeType::BOOL: return 1;
				case VertexAttributeType::FLOAT: return 4;
				case VertexAttributeType::FLOAT2: return 4 * 2;
				case VertexAttributeType::FLOAT3: return 4 * 3;
				case VertexAttributeType::FLOAT4: return 4 * 4;
				case VertexAttributeType::UINT:
				case VertexAttributeType::INT: return 4;
				case VertexAttributeType::INT2:
				case VertexAttributeType::UINT2: return 4 * 2;
				case VertexAttributeType::INT3:
				case VertexAttributeType::UINT3: return 4 * 3;
				case VertexAttributeType::INT4:
				case VertexAttributeType::UINT4: return 4 * 4;
				case VertexAttributeType::MAT3:
				case VertexAttributeType::MAT4: return 4 * 4 * 4;
			}
			ASSERT(false, "Unknown VertexBufferDataType");
			return 0;
		}

		uint32_t toBaseType(VertexAttributeType type) {
			switch (type) {
				case VertexAttributeType::BOOL: return GL_BOOL;
				case VertexAttributeType::FLOAT:
				case VertexAttributeType::FLOAT2:
				case VertexAttributeType::FLOAT3:
				case VertexAttributeType::FLOAT4:
				case VertexAttributeType::MAT3:
				case VertexAttributeType::MAT4: return GL_FLOAT;
				case VertexAttributeType::UINT: return GL_UNSIGNED_INT;
				case VertexAttributeType::INT:
				case VertexAttributeType::INT2:
				case VertexAttributeType::INT3:
				case VertexAttributeType::INT4: return GL_INT;
				case VertexAttributeType::UINT2:
				case VertexAttributeType::UINT3:
				case VertexAttributeType::UINT4: return GL_UNSIGNED_INT;
			}
			ASSERT(false, "Unknown VertexBufferDataType");
			return 0;
		}

		uint32_t toComponentCount(VertexAttributeType type) {
			switch (type) {
				case VertexAttributeType::BOOL: return 1;
				case VertexAttributeType::FLOAT: return 1;
				case VertexAttributeType::FLOAT2: return 2;
				case VertexAttributeType::FLOAT3: return 3;
				case VertexAttributeType::FLOAT4: return 4;
				case VertexAttributeType::UINT: return 1;
				case VertexAttributeType::INT: return 1;
				case VertexAttributeType::INT2: return 2;
				case VertexAttributeType::UINT2: return 2;
				case VertexAttributeType::INT3: return 3;
				case VertexAttributeType::UINT3: return 3;
				case VertexAttributeType::INT4: return 4;
				case VertexAttributeType::UINT4: return 4;
				case VertexAttributeType::MAT3: return 9;
				case VertexAttributeType::MAT4: return 16;
			}
			ASSERT(false, "Unknown VertexBufferDataType");
			return 0;
		}

		uint32_t isIntegerType(VertexAttributeType type) {
			switch (type) {
				case VertexAttributeType::FLOAT:
				case VertexAttributeType::FLOAT2:
				case VertexAttributeType::FLOAT3:
				case VertexAttributeType::FLOAT4: return false;
				case VertexAttributeType::UINT:
				case VertexAttributeType::INT:
				case VertexAttributeType::INT2:
				case VertexAttributeType::INT3:
				case VertexAttributeType::INT4:
				case VertexAttributeType::UINT2:
				case VertexAttributeType::UINT3:
				case VertexAttributeType::UINT4:
				case VertexAttributeType::BOOL: return true;
			}
			ASSERT(false, "Unknown VertexBufferDataType");
			return 0;
		}
	}

	VertexBufferLayout::VertexBufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements) {
		calculateOffsetsAndStride();
	}

	uint32_t VertexBufferLayout::getTotalComponentCountSize() {
		uint32_t count = 0;
		for (BufferElement& element : m_elements) {
			count += VertexAttribute::toComponentCount(element.m_type) * sizeof(VertexAttribute::toBaseType(element.m_type));
		}
		return count;
	}

	void VertexBufferLayout::setVertexAttributeLocations(uint32_t attributeIndex) {
		
	}
}