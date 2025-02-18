#pragma once
#include "utils/datastructures/buffer.h"

namespace emerald {
	enum BufferUsage : uint32_t {
		STATIC = 0x88E4,			//GL_STATIC_DRAW
		DYNAMIC = 0x88E8,			//GL_DYNAMIC_DRAW
		STREAM = 0x88E0				//GL_STREAM_DRAW 
	};

	enum class BufferTarget : uint32_t {
		ARRAY = 0x8892,				//GL_ARRAY_BUFFER
		ELEMENTARRAY = 0x8893		//GL_ELEMENT_ARRAY_BUFFER
	};;

	enum class BufferAccess : uint32_t {
		READ = 0x88B8,				//GL_READ_ONLY
		WRITE = 0x88B9,				//GL_WRITE_ONLY
		READWRITE = 0x88BA			//GL_READ_WRITE
	};

	class GLBuffer : public RefCounted {
	protected:
		uint32_t m_handle = 0;
		Buffer<byte> m_data;
		BufferUsage m_usage;
		BufferTarget m_target;

	public:
		GLBuffer(BufferTarget target, BufferUsage usage);
		GLBuffer(BufferTarget target, const byte* data, uint32_t size, BufferUsage usage);
		GLBuffer(GLBuffer&& other) noexcept;
		GLBuffer& operator=(GLBuffer&& other) noexcept;
		virtual ~GLBuffer();

		void setData(const byte* data, uint32_t size, uint32_t offset = 0);

		void bind() const;

		uint32_t handle() const { return m_handle; }
		uint32_t getCount() const { return m_data.size() / sizeof(uint32_t); }
		uint32_t getSize() const { return m_data.size(); }
	};

	class VertexBuffer : public GLBuffer {
	public:
		VertexBuffer(BufferUsage usage);
		VertexBuffer(const byte* data, uint32_t size, BufferUsage usage);
	};

	class IndexBuffer : public GLBuffer {
	public:
		IndexBuffer(BufferUsage usage);
		IndexBuffer(const byte* data, uint32_t size, BufferUsage usage);
	};
}