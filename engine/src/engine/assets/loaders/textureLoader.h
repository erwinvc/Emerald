#pragma once
#include "assetLoader.h"
#include "../metadata/assetMetadata.h"
#include "graphics/textures/textureDesc.h"
#include "utils/datastructures/buffer.h"

namespace emerald {
	class TextureLoader : public AssetLoader {
	public:
		// Load from file on disk
		TextureLoader(TextureDesc desc, const std::filesystem::path& path, bool flip)
			: m_desc(desc), m_path(path), m_channelCount(0), m_width(0), m_height(0), m_buffer(), m_flip(flip),
			m_loadFromMemory(false), m_data(nullptr), m_dataSize(0) {
			if (m_desc.name.empty()) {
				m_desc.name = path.filename().string();
			}
		}

		// Load from embedded file
		TextureLoader(TextureDesc desc, const byte* data, size_t dataSize, bool flip)
			: m_desc(desc), m_path(""), m_channelCount(0), m_width(0), m_height(0), m_buffer(), m_flip(flip),
			m_loadFromMemory(true), m_data(data), m_dataSize(dataSize) {
		}

		Ref<Asset> loadAndInvalidate();

	protected:

		virtual bool onBeginLoad() override;
		virtual Ref<Asset> onFinishLoad() override;

	private:
		std::filesystem::path m_path;
		TextureDesc m_desc;
		uint32_t m_channelCount;
		uint32_t m_width;
		uint32_t m_height;
		Buffer<byte> m_buffer;
		bool m_flip;

		bool m_loadFromMemory;
		const byte* m_data;
		size_t m_dataSize;
	};
}