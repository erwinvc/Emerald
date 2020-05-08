#pragma once

//ElementsRenderer uses glDrawElements to draw m_amount * T stored in the buffer.
//Data is stored per vertex. In the InstancedRenderer it's stored per instance.
template<typename T>
class ElementsRenderer {
private:
	bool m_started;
	bool m_ended;
	AssetRef<Mesh> m_mesh = nullptr;
	const int m_vertexCount;
	const int m_indicesCount;
	const int m_maxObjects;
	ManagedRef<VertexBuffer> m_vertexBuffer = nullptr;
	int m_amount = 0;
	T* m_buffer = nullptr;
	T* m_mappedBuffer = nullptr;
	BufferLayout m_layout;

	void Initialize() {
		m_buffer = new T[m_vertexCount * m_maxObjects];
		
		m_vertexBuffer = NEW(VertexBuffer((float*)m_buffer, m_vertexCount * m_maxObjects, m_layout, GL_DYNAMIC_DRAW));
		m_mesh->GetVAO()->AddBuffer(m_vertexBuffer);
		m_mesh->GetVAO()->ApplyLayouts();
	}

public:
	void Begin() {
		ASSERT(m_ended, "Call ElementsRenderer::End before calling ElementsRenderer::Begin");
		m_amount = 0;
		m_ended = false;
		m_started = true;

		m_mappedBuffer = m_vertexBuffer->Map<T>();
	}

	void Submit(T* data, int vertexCount, int count) {
		ASSERT(m_amount + (count * m_indicesCount) <= m_maxObjects * m_indicesCount, "ElementsRenderer buffer full!");
		memcpy(m_mappedBuffer, data, vertexCount * sizeof(T));
		m_mappedBuffer += vertexCount;
		m_amount += count * m_indicesCount;
	}

	void End() {
		ASSERT(m_started, "Call ElementsRenderer::Begin before calling ElementsRenderer::End");
		m_vertexBuffer->Unmap();
		m_started = false;
		m_ended = true;
	}

	void Draw(uint mode = GL_TRIANGLES) {
		ASSERT(m_ended, "Call ElementsRenderer::End before calling ElementsRenderer::Draw");
		m_mesh->GetMaterial()->Bind();
		m_mesh->DrawCount(m_amount, mode);
	}

	ElementsRenderer(AssetRef<Mesh> mesh, int vertexCount, int indicesCount, int maxObjects, const BufferLayout& layout) : m_started(false), m_ended(true), m_vertexCount(vertexCount), m_indicesCount(indicesCount), m_maxObjects(maxObjects), m_layout(layout), m_mesh(mesh->Copy()) { Initialize(); }
	~ElementsRenderer() { delete[] m_buffer; DELETE(m_mesh); }
};