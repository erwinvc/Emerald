#pragma once

class VertexArray {
private:
	GLuint m_arrayID;
	vector<ManagedRef<VertexBuffer>> m_buffers;

public:
	VertexArray();
	~VertexArray();

	void ApplyLayouts() {
		Bind();
		uint32 attributeIndex = 0;
		for (int i = 0; i < m_buffers.size(); i++) {
			m_buffers[i]->ApplyLayout(attributeIndex);
			attributeIndex += m_buffers[i]->GetLayout().GetElements().size();
		}
		Unbind();
	}

	int GetBufferCount() { return m_buffers.size(); }
	vector<ManagedRef<VertexBuffer>> GetBuffers() { return m_buffers; }
	ManagedRef<VertexBuffer> GetBuffer(int index) { return m_buffers.at(index); }
	void AddBuffer(ManagedRef<VertexBuffer> buffer);
	void Bind();
	void Unbind();
};