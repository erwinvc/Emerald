#pragma once
#include "vertexBufferLayout.h"
#include "util/buffer.h"

namespace emerald {
	class VertexBuffer {
	private:
		uint32_t m_handle;
		Buffer<byte> m_data;
		BufferUsage m_usage;

	public:
		VertexBuffer() : m_handle(0), m_data(), m_usage(BufferUsage::STATIC_DRAW) {}
		explicit VertexBuffer(const byte* data, uint32_t size, BufferUsage usage = STATIC_DRAW);

		~VertexBuffer();

		void setData(const byte* data, uint32_t size, uint32_t offset = 0);

		uint32_t handle() const { return m_handle; }
		uint32_t size() const { return m_data.size(); }

		void bind() const;

		template<typename T>
		T* map() {
			T* toRet = (T*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY | GL_MAP_UNSYNCHRONIZED_BIT);
			return toRet;
		}

		void unmap() {
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
	};
}