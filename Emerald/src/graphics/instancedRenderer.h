#pragma once

template<typename T>
class InstancedRenderer {
private:
	bool m_started;
	bool m_ended;
	AssetRef<Mesh> m_mesh = nullptr;
	const int m_maxObjects = 0;
	ManagedRef<VertexBuffer> m_offsetsBuffer = nullptr;
	int m_amount = 0;
	T* m_offsets = nullptr;
	T* m_buffer = nullptr;
	BufferLayout m_layout;

	void Initialize() {
		ASSERT(m_mesh, "InstancedRenderer mesh is a null pointer");
		m_offsets = new T[m_maxObjects];
		m_offsetsBuffer = NEW(VertexBuffer((float*)m_offsets, m_maxObjects, m_layout, GL_DYNAMIC_DRAW));
		m_mesh->GetVAO()->AddBuffer(m_offsetsBuffer);
		m_mesh->GetVAO()->ApplyLayouts();
	}

public:
	void Begin() {
		ASSERT(m_ended, "Call InstancedRenderer::End before calling InstancedRenderer::Begin");
		m_amount = 0;
		m_ended = false;
		m_started = true;

		m_offsetsBuffer->Bind();
		GL(m_buffer = (T*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		m_offsetsBuffer->Unbind();
	}

	void Submit(T& offset) {
		if (m_amount >= m_maxObjects) return;
		*m_buffer = offset;
		m_buffer++;
		m_amount++;
	}

	void End() {
		ASSERT(m_started, "Call InstancedRenderer::Begin before calling InstancedRenderer::End");
		m_offsetsBuffer->Bind();
		GL(glUnmapBuffer(GL_ARRAY_BUFFER));
		m_started = false;
		m_ended = true;
	}

	void Set(vector<T>& entries, int count) {
		ASSERT(!m_started, "Call InstancedRenderer::End before calling InstancedRenderer::Set");
		m_offsetsBuffer->Bind();
		GL(glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(T), (void*)entries.data()));
		m_offsetsBuffer->Unbind();
		m_amount = count;
		m_ended = true;
	}

	void Set(T* entries, int count) {
		ASSERT(!m_started, "Call InstancedRenderer::End before calling InstancedRenderer::Set");
		m_offsetsBuffer->Bind();
		GL(glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(T), (void*)entries));
		m_offsetsBuffer->Unbind();
		m_amount = count;
		m_ended = true;
	}

	void Draw(AssetRef<Shader> shader, uint mode = GL_TRIANGLES) {
		ASSERT(m_ended, "Call InstancedRenderer::End before calling InstancedRenderer::Draw");
		m_mesh->GetMaterial()->Bind(shader);
		m_mesh->DrawInstanced(m_amount, mode);
	}

	void Draw(uint mode = GL_TRIANGLES) {
		ASSERT(m_ended, "Call InstancedRenderer::End before calling InstancedRenderer::Draw");
		m_mesh->DrawInstanced(m_amount, mode);
	}

	InstancedRenderer(AssetRef<Mesh> mesh, int maxObjects, const BufferLayout& layout) : m_started(false), m_ended(true), m_maxObjects(maxObjects), m_layout(layout), m_mesh(mesh->Copy()) { Initialize(); }
	~InstancedRenderer() { delete[] m_offsets; DELETE(m_mesh); }
};