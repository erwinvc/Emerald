#pragma once

class VertexArray;

enum class ShaderDataType {
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

static uint32 ShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
	case ShaderDataType::Bool: return 1;
	case ShaderDataType::Float: return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;
	case ShaderDataType::Int: return 4;
	case ShaderDataType::Int2: return 4 * 2;
	case ShaderDataType::Int3: return 4 * 3;
	case ShaderDataType::Int4: return 4 * 4;
	}
	LOG_ERROR("[~bShaders~x] Unknow ShaderDataType!");
	return 0;
}

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	switch (type) {
	case ShaderDataType::Bool: return GL_BOOL;
	case ShaderDataType::Float:
	case ShaderDataType::Float2:
	case ShaderDataType::Float3:
	case ShaderDataType::Float4: return GL_FLOAT;
	case ShaderDataType::Int:
	case ShaderDataType::Int2:
	case ShaderDataType::Int3:
	case ShaderDataType::Int4: return GL_INT;
	}
	LOG_ERROR("[~bShaders~x] Unknow ShaderDataType!");
	return 0;
}
struct BufferElement {
	String m_name;
	ShaderDataType m_type;
	uint32 m_size;
	uint32 m_offset;
	uint32 m_bufferIndex;
	bool m_divisor;
	bool m_normalized;

	BufferElement() {}
	BufferElement(ShaderDataType type, const String& name, uint32 bufferIndex, bool divisor = false) : m_name(name), m_type(type), m_size(ShaderDataTypeSize(type)), m_offset(0), m_bufferIndex(bufferIndex), m_divisor(divisor), m_normalized(false) {}

	uint32 GetComponentCount() const {
		switch (m_type) {
		case ShaderDataType::Bool: return 1;
		case ShaderDataType::Float: return 1;
		case ShaderDataType::Float2: return 2;
		case ShaderDataType::Float3: return 3;
		case ShaderDataType::Float4: return 4;
		case ShaderDataType::Int: return 1;
		case ShaderDataType::Int2: return 2;
		case ShaderDataType::Int3: return 3;
		case ShaderDataType::Int4: return 4;
		}
		LOG_ERROR("[~bShaders~x] Unknow ShaderDataType!");
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
		for (BufferElement& element : m_elements) count += element.GetComponentCount() * sizeof(ShaderDataTypeToOpenGLBaseType(element.m_type));
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