#include "eepch.h"
#include "textureLoader.h"
#include "utils/system/fileSystem.h"
#include <stb_image.h>
#include "graphics/textures/texture.h"
#include "graphics/core/renderer.h"

namespace emerald {
	bool TextureLoader::onBeginLoad() {
		byte* data = nullptr;
		int channelCount;
		int width, height;

		stbi_set_flip_vertically_on_load(m_flip);

		if (!m_loadFromMemory) {
			if (!FileSystem::doesFileExist(m_path)) {
				Log::fatal("[Texture] texture file at {} does not exist.", m_path.string());
				return false;
			}

			data = stbi_load(m_path.string().c_str(), &width, &height, &channelCount, 4);
		} else {
			if (m_data == nullptr || m_dataSize == 0) {
				Log::fatal("[TextureLoader] Memory data is null or size is zero.");
				return false;
			}

			data = stbi_load_from_memory((const stbi_uc*)m_data, (int)m_dataSize, &width, &height, &channelCount, 4);
		}
		if (!data) {
			Log::error("[TextureLoader] Failed to load image from file: {}", m_path.string());
			return false;
		}

		if (channelCount < 1 || channelCount > 4) {
			Log::error("[TextureLoader] Unsupported image channel count ({})", channelCount, m_path.string());
			stbi_image_free(data);
			return false;
		}

		uint32_t size = 4 * width * height;
		m_channelCount = 4;
		m_width = (uint32_t)width;
		m_height = (uint32_t)height;
		m_buffer = Buffer<byte>::copy(data, size);

		stbi_image_free(data);
		return true;
	}

	Ref<Asset> TextureLoader::onFinishLoad() {
		Ref<Texture> texture = Ref<Texture>::create(m_desc, m_width, m_height, m_buffer.data(), m_buffer.size());
		Renderer::submit([instance = Ref<Texture>(texture)] {instance->invalidate(); });
		return texture;
	}
}