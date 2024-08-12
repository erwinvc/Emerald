#pragma once
#include "graphics/buffers/vertexBuffer.h"

namespace emerald {
	class VertexArray : public RefCounted {
	private:
		VertexBufferLayout m_layout;
		GLuint m_handle;
		bool m_validated;

		std::vector<std::shared_ptr<VertexBuffer>> m_buffers;

	public:
		VertexArray(VertexBufferLayout layout);
		~VertexArray();

		void addBuffer(std::shared_ptr<VertexBuffer> buffer);
		void validate();
		void bind() const;

		inline uint32_t getBufferCount() { return (uint32_t)m_buffers.size(); }
		inline std::vector<std::shared_ptr<VertexBuffer>>& getBuffers() { return m_buffers; }
		inline std::shared_ptr<VertexBuffer> getBuffer(int index) { return m_buffers.at(index); }
	};
}