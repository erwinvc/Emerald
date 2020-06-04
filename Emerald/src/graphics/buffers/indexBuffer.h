#pragma once

class IndexBuffer : public Ref<IndexBuffer> {
private:
	GLuint m_bufferID;
	GLuint m_count;
public:
	IndexBuffer(uint* data, uint32 count);
	~IndexBuffer();
	inline GLuint GetCount() { return m_count; }

	void Bind();
	void Unbind();

	void SetData(uint* data, uint32 count, GLenum usage = GL_STATIC_DRAW);

	void Draw(uint mode);
	void Draw(uint32 count, uint mode);
	void DrawInstanced(uint32 amount, uint mode = GL_TRIANGLES);
};

