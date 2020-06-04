#pragma once
class VertexBuffer : public Ref<VertexBuffer> {
private:
	uint32 m_bufferID;
	BufferLayout m_layout;
	GLfloat* m_data;
public:
	VertexBuffer() {}
	
	VertexBuffer(float* data, uint32 vertexCount, BufferLayout layout, GLenum usage = GL_STATIC_DRAW);

	~VertexBuffer();
	void ApplyLayout(uint32 attributeIndex);

	void SetData(const void* data, uint32 vertexCount, GLenum usage = GL_STATIC_DRAW);
	
	inline const BufferLayout& GetLayout() { return m_layout; }
	inline void SetLayout(BufferLayout& layout) { m_layout = layout; }

	inline uint32 GetHandle() { return m_bufferID; }

	inline void Bind() {
		GL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID));
	}
	inline void Unbind() {
		GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	template<typename T>
	T* Map() {
		Bind();
		T* mapped;
		GL(mapped = (T*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY | GL_MAP_UNSYNCHRONIZED_BIT));
		return mapped;
	}

	void Unmap() {
		Bind();
		GL(glUnmapBuffer(GL_ARRAY_BUFFER));
	}

};

