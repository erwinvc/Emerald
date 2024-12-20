#include "eepch.h"
#include "textureLoader.h"
#include "utils/system/fileSystem.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include "graphics/textures/texture.h"

namespace emerald {
	void TextureLoader::asyncLoad() {
		if (!FileSystem::doesFileExist(m_path)) {
			Log::fatal("[Texture] texture file at {} does not exist.", m_path.string());
			return;
		}

		int channelCount;
		int width, height;

		//stbi_set_flip_vertically_on_load(desc.m_flip);
		byte* data = stbi_load(m_path.string().c_str(), &width, &height, &channelCount, 4);

		//if (bpc != 3 && bpc != 4) {
		//	LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
		//	stbi_image_free(data);
		//	return false;
		//}

		if (channelCount < 1 || channelCount > 4) Log::error("[Texture] unsupported image channel count ({}) {}.", channelCount, m_path.string());

		uint32_t size = channelCount * width * height;

		if (data) {
			m_channelCount = channelCount;
			m_width = width;
			m_height = height;
			m_buffer = Buffer<byte>::copy(data, size);
			stbi_image_free(data);
			Log::info("[Texture] loaded {}", m_path.string());
		} else Log::error("[Texture] failed to load {}", m_path.string());
	}

	Ref<Asset> TextureLoader::syncLoad() {
		return Ref<Texture>::create(m_desc, m_width, m_height, m_buffer.data(), m_buffer.size(), TextureDataType::RAW);
	}
}