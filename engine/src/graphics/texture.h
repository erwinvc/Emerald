#pragma once
#include "stdincl.h"
#include "util/buffer.h"
#include "textureDesc.h"
#include "assrt.h"

namespace emerald {
	enum class TextureDataType {
		RAW,
		FILE
	};

	class Texture : public RefCounted {
	private:
		TextureDesc m_desc;
		Buffer<byte> m_buffer;
		uint32_t m_handle = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_channelCount = 0;
		uint8_t m_mipmapCount = 0;
		void cleanup() const;

	public:
		Texture() : m_desc(TextureDesc()) {}
		Texture(TextureDesc desc, std::string path);
		Texture(TextureDesc desc, uint32_t width, uint32_t height, const byte* data, uint32_t dataSize, TextureDataType textureDataType);
		Texture(TextureDesc desc, uint32_t width, uint32_t height);
		~Texture();

		void invalidate();

		uint32_t handle() const { return m_handle; }

		void bind(uint32_t slot = 0) const;
		void unbind(uint32_t slot = 0) const;
		void resize(uint32_t width, uint32_t height);

		inline const uint32_t getWidth() const { return m_width; }
		inline const uint32_t getHeight() const { return m_height; }
		void saveAsPNG(const std::string& file);

		uint32_t getImageMemorySize() {
			return m_desc.getChannelCount() * m_width * m_height;
		}

		Buffer<byte>& getBuffer() {
			ASSERT(m_desc.m_readWrite, "Read write is not enabled for this buffer");
			return m_buffer;
		}
	};
}
