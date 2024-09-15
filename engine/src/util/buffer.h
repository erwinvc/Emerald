#pragma once
#include "stdincl.h"

namespace emerald {
	enum BufferUsage {
		STATIC_DRAW = 0x88E4,	//GL_STATIC_DRAW
		DYNAMIC_DRAW = 0x88E8,	//GL_DYNAMIC_DRAW
		STREAM_DRAW = 0x88E0	//GL_STREAM_DRAW 
	};

	template<typename T>
	class Buffer {
	private:
		T* m_data;
		uint32_t m_size;

		void cleanup() {
			delete[] m_data;
			m_data = nullptr;
			m_size = 0;
		}

	public:
		Buffer() : m_data(nullptr), m_size(0) {}

		explicit Buffer(uint32_t size) : m_data(nullptr), m_size(size) {
			m_data = new T[size]();
		}

		Buffer(const Buffer& other) : m_data(nullptr), m_size(other.m_size) {
			if (m_size > 0) {
				m_data = new T[m_size];
				std::copy(other.m_data, other.m_data + m_size, m_data);
			}
		}

		Buffer(Buffer&& other) noexcept : m_data(other.m_data), m_size(other.m_size) {
			other.m_data = nullptr;
			other.m_size  = 0;
		}

		Buffer& operator=(const Buffer& other) {
			if (this != &other) {
				cleanup();
				m_size = other.m_size;
				if (m_size > 0) {
					m_data = new T[m_size];
					std::copy(other.m_data, other.m_data + m_size, m_data);
				}
			}
			return *this;
		}

		Buffer& operator=(Buffer&& other) noexcept {
			if (this != &other) {
				cleanup();
				m_data = other.m_data;
				m_size = other.m_size;
				other.m_data = nullptr;
				other.m_size = 0;
			}
			return *this;
		}

		~Buffer() { cleanup(); }

		static Buffer copy(const T* data, uint32_t size) {
			Buffer buffer(size);
			std::copy(data, data + size, buffer.m_data);
			return buffer;
		}

		void clear() {
			cleanup();
		}

		void reserve(uint32_t new_size) {
			if (new_size > m_size) {
				T* new_data = new T[new_size];
				if (m_data) {
					std::copy(m_data, m_data + m_size, new_data);
					delete[] m_data;
				}
				m_data = new_data;
				m_size = new_size;
			}
		}

		void zeroInitialize() {
			if (m_data)
				memset(m_data, 0, m_size);
		}

		T& operator[](uint32_t index) { return at(index); }
		const T& operator[](uint32_t index) const { return at(index); }

		T& at(uint32_t index) {
			if (index >= m_size) throw std::out_of_range("Buffer index out of range");
			return m_data[index];
		}

		const T& at(uint32_t index) const {
			if (index >= m_size) throw std::out_of_range("Buffer index out of range");
			return m_data[index];
		}

		template<typename T>
		T& read(uint32_t offset = 0) const {
			return *(T*)((byte*)m_data + offset);
		}

		template<typename T>
		void readArray(T* dst, size_t offset, size_t count) const {
			std::memcpy(dst, &m_data[offset], sizeof(T) * count);
		}

		void write(void* data, uint32_t size, uint32_t offset = 0) {
			ASSERT(offset + size <= m_size, "Out of buffer bounds");
			memcpy((byte*)m_data + offset, data, size);
		}

		explicit operator bool() const noexcept { return m_data != nullptr; }

		uint32_t size() const noexcept { return m_size; }
		T* data() noexcept { return m_data; }
		const T* data() const noexcept { return m_data; }

		T* begin() noexcept { return m_data; }
		const T* begin() const noexcept { return m_data; }
		T* end() noexcept { return m_data + m_size; }
		const T* end() const noexcept { return m_data + m_size; }
	};
}