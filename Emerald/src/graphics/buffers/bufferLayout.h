#pragma once

class VertexArray;

enum class VertexBufferDataType {
	None = 0,
	Bool,
	Float,
	Float2,
	Float3,
	Float4,
	Int,
	Int2,
	Int3,
	Int4
};

static uint32 VertexBufferDataTypeToSize(VertexBufferDataType type) {
	switch (type) {
	case VertexBufferDataType::Bool: return 1;
	case VertexBufferDataType::Float: return 4;
	case VertexBufferDataType::Float2: return 4 * 2;
	case VertexBufferDataType::Float3: return 4 * 3;
	case VertexBufferDataType::Float4: return 4 * 4;
	case VertexBufferDataType::Int: return 4;
	case VertexBufferDataType::Int2: return 4 * 2;
	case VertexBufferDataType::Int3: return 4 * 3;
	case VertexBufferDataType::Int4: return 4 * 4;
	}
	LOG_ERROR("[~cBuffers~x] Unknow VertexBufferDataType!");
	return 0;
}

static GLenum VertexBufferDataTypeToOpenGLBaseType(VertexBufferDataType type) {
	switch (type) {
	case VertexBufferDataType::Bool: return GL_BOOL;
	case VertexBufferDataType::Float:
	case VertexBufferDataType::Float2:
	case VertexBufferDataType::Float3:
	case VertexBufferDataType::Float4: return GL_FLOAT;
	case VertexBufferDataType::Int:
	case VertexBufferDataType::Int2:
	case VertexBufferDataType::Int3:
	case VertexBufferDataType::Int4: return GL_INT;
	}
	LOG_ERROR("[~cBuffers~x] Unknow VertexBufferDataType!");
	return 0;
}
struct BufferElement {
	String m_name;
	VertexBufferDataType m_type;
	uint32 m_size;
	uint32 m_offset;
	uint32 m_bufferIndex;
	bool m_divisor;
	bool m_normalized;

	BufferElement() : m_name(""), m_type(VertexBufferDataType::Bool), m_size(0), m_offset(0), m_bufferIndex(0), m_divisor(false), m_normalized(false) {}
	BufferElement(VertexBufferDataType type, const String& name, uint32 bufferIndex, bool divisor = false) : m_name(name), m_type(type), m_size(VertexBufferDataTypeToSize(type)), m_offset(0), m_bufferIndex(bufferIndex), m_divisor(divisor), m_normalized(false) {}

	uint32 GetComponentCount() const {
		switch (m_type) {
		case VertexBufferDataType::Bool: return 1;
		case VertexBufferDataType::Float: return 1;
		case VertexBufferDataType::Float2: return 2;
		case VertexBufferDataType::Float3: return 3;
		case VertexBufferDataType::Float4: return 4;
		case VertexBufferDataType::Int: return 1;
		case VertexBufferDataType::Int2: return 2;
		case VertexBufferDataType::Int3: return 3;
		case VertexBufferDataType::Int4: return 4;
		}
		LOG_ERROR("[~cBuffers~x] Unknow VertexBufferDataType!");
		return 0;
	}
};

class BufferLayout {
public:
	BufferLayout() {}
	BufferLayout(const initializer_list<BufferElement>& elements) : m_elements(elements) {
		CalculateOffsetsAndStride();
	}


	uint32 GetTotalComponentCountSize() {
		uint32 count = 0;
		for (BufferElement& element : m_elements) count += element.GetComponentCount() * sizeof(VertexBufferDataTypeToOpenGLBaseType(element.m_type));
		return count;
	}
	void Apply(uint32 attributeIndex);

	vector<BufferElement>::iterator begin() { return m_elements.begin(); }
	vector<BufferElement>::iterator end() { return m_elements.end(); }
	inline const vector<BufferElement>& GetElements() const { return m_elements; }
	inline uint32 GetStride() const { return m_stride; }

private:
	void CalculateOffsetsAndStride() {
		uint32 offset = 0;
		m_stride = 0;
		for (auto& element : m_elements) {
			element.m_offset = offset;
			offset += element.m_size;
			m_stride += element.m_size;
		}
	}

private:
	uint32 m_stride = 0;
	vector<BufferElement> m_elements;
};