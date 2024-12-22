#pragma once
#include "assetLoader.h"
#include "../metadata/assetMetadata.h"
#include "graphics/textures/textureDesc.h"
#include "utils/datastructures/buffer.h"

namespace emerald {
	class TextureLoader : public AssetLoader {
	public:
		TextureLoader(TextureDesc desc, const std::filesystem::path& path, bool flip) : m_desc(desc), m_path(path), m_channelCount(0), m_width(0), m_height(0), m_buffer(), m_flip(flip) {}

		virtual bool onBeginLoad() override;
		virtual Ref<Asset> onFinishLoad() override;
		Ref<Asset> syncLoadAndInvalidate();

	private:
		std::filesystem::path m_path;
		TextureDesc m_desc;
		uint32_t m_channelCount;
		uint32_t m_width;
		uint32_t m_height;
		Buffer<byte> m_buffer;
		bool m_flip;
	};
}