#pragma once
#include "stdincl.h"

namespace emerald {
	class VertexArray;

	enum class VertexAttributeType {
		None = 0,
		BOOL,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		UINT,
		INT,
		INT2,
		UINT2,
		INT3,
		UINT3,
		INT4,
		UINT4,
		MAT3,
		MAT4
	};

	namespace VertexAttribute {
		uint32_t toSize(VertexAttributeType type);
		uint32_t toBaseType(VertexAttributeType type);
		uint32_t toComponentCount(VertexAttributeType type);
		uint32_t isIntegerType(VertexAttributeType type);
	}

	struct BufferElement {
		std::string m_name;
		VertexAttributeType m_type;
		uint32_t m_size = 0;
		uint32_t m_offset = 0;
		uint32_t m_bufferIndex = 0;

		bool m_divisor = false;
		bool m_normalized = false;

		BufferElement() = default;
		BufferElement(VertexAttributeType type, const std::string& name, uint32_t bufferIndex = 0, bool divisor = false, bool normalized = false)
			: m_name(name), m_type(type), m_size(VertexAttribute::toSize(type)), m_offset(0), m_divisor(divisor), m_bufferIndex(bufferIndex), m_normalized(normalized) {
		}
	};

	class VertexBufferLayout {
	private:
		uint32_t m_stride = 0;
		std::vector<BufferElement> m_elements;

		void calculateOffsetsAndStride() {
			uint32_t offset = 0;
			m_stride = 0;
			for (auto& element : m_elements) {
				element.m_offset = offset;
				offset += element.m_size;
				m_stride += element.m_size;
			}
		}

	public:
		VertexBufferLayout(const std::initializer_list<BufferElement>& elements);
		VertexBufferLayout() {}

		uint32_t getTotalComponentCountSize();
		void setVertexAttributeLocations(uint32_t attributeIndex);

		inline std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		inline std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		inline const std::vector<BufferElement>& elements() const { return m_elements; }
		inline uint32_t stride() const { return m_stride; }
	};
}