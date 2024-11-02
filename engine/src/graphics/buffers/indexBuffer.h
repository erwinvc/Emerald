#pragma once

#include "utils/datastructures/buffer.h"
namespace emerald {
	class IndexBuffer : public RefCounted {
	private:
		uint32_t m_handle = 0;
		Buffer<byte> m_data;

	public:
		IndexBuffer() : m_handle(0), m_data() {}
		IndexBuffer(const byte* data, uint32_t size);
		~IndexBuffer();

		void setData(const byte* data, uint32_t size, uint32_t offset = 0);
		void bind() const;

		uint32_t handle() const { return m_handle; }
		uint32_t getCount() const { return m_data.size() / sizeof(uint32_t); }
		uint32_t getSize() const { return m_data.size(); }
	};
}