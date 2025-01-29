#pragma once
#include "utils/datastructures/buffer.h"
#include "textureDesc.h"
#include "engine/assets/core/asset.h"

namespace emerald {
	class Texture : public Asset {
	private:
		TextureDesc m_desc;
		Buffer<byte> m_buffer;
		uint32_t m_handle = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		//uint32_t m_channelCount = 0;
		uint8_t m_mipmapCount = 0;
		bool m_dirty = true;
		void cleanup() const;

	public:
		Texture() : m_desc(TextureDesc()) {}
		Texture(TextureDesc desc, uint32_t width, uint32_t height, const byte* data, uint32_t dataSize);
		Texture(TextureDesc desc, uint32_t width, uint32_t height);
		~Texture();

		const TextureDesc& descriptor() const { return m_desc; }

		void invalidate();
		void submitInvalidate();

		uint32_t handle() const { return m_handle; }

		void bind(uint32_t slot = 0) const;
		void unbind(uint32_t slot = 0) const;
		void resize(uint32_t width, uint32_t height);

		const uint32_t getWidth() const { return m_width; }
		const uint32_t getHeight() const { return m_height; }
		void saveAsPNG(const std::string& file);

		uint32_t getImageMemorySize() {
			return m_desc.getBytesPerPixel() * m_width * m_height;
		}

		Buffer<byte>& getBuffer() {
			ASSERT(m_desc.readWrite, "Read write is not enabled for this buffer");
			return m_buffer;
		}
	};
}
