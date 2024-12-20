#pragma once
#include "assetLoader.h"
#include "../metadata/assetMetadata.h"
#include "graphics/textures/textureDesc.h"
#include "utils/datastructures/buffer.h"

namespace emerald {
	class TextureLoader : public AssetLoader {
	public:
		TextureLoader(TextureDesc desc, const std::filesystem::path& path) : m_desc(desc), m_path(path), m_channelCount(0), m_width(0), m_height(0), m_buffer() {}

		virtual void asyncLoad() override;
		virtual Ref<Asset> syncLoad() override;

	private:
		std::filesystem::path m_path;
		TextureDesc m_desc;
		uint32_t m_channelCount;
		uint32_t m_width;
		uint32_t m_height;
		Buffer<byte> m_buffer;
	};
}