#pragma once

class VertexArray : public Ref<VertexArray> {
private:
	GLuint m_arrayID;
	vector<ManagedRef<VertexBuffer>> m_buffers;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(ManagedRef<VertexBuffer> buffer);
	void ApplyLayouts();

	void Bind();
	void Unbind();
	
	inline int GetBufferCount() { return m_buffers.size(); }
	inline vector<ManagedRef<VertexBuffer>> GetBuffers() { return m_buffers; }
	inline ManagedRef<VertexBuffer> GetBuffer(int index) { return m_buffers.at(index); }
};