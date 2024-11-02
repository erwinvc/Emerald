#pragma once
#include "graphics/buffers/vertexBuffer.h"

namespace emerald {
	class VertexArray : public RefCounted {
	private:
		VertexBufferLayout m_layout;
		uint32_t m_handle;
		bool m_validated;

		std::vector<Ref<VertexBuffer>> m_buffers;

	public:
		VertexArray(VertexBufferLayout layout);
		~VertexArray();

		void addBuffer(const Ref<VertexBuffer>& buffer);
		void validate();
		void bind() const;
		uint32_t handle() const { return m_handle; }

		inline uint32_t getBufferCount() { return (uint32_t)m_buffers.size(); }
		inline std::vector<Ref<VertexBuffer>>& getBuffers() { return m_buffers; }
		inline const Ref<VertexBuffer>& getBuffer(int index) { return m_buffers.at(index); }
	};
}