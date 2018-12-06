#pragma once


class VertexArray
{
private:
	GLuint m_arrayID;
	vector<Buffer*> m_buffers;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(Buffer* buffer, GLuint index, bool divisor);
	void Bind();
	void Unbind();
};