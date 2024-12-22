#include "eepch.h"
#include "textureLoader.h"
#include "utils/system/fileSystem.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include "graphics/textures/texture.h"
#include "graphics/core/renderer.h"

namespace emerald {
	bool TextureLoader::onBeginLoad() {
		if (!FileSystem::doesFileExist(m_path)) {
			Log::fatal("[Texture] texture file at {} does not exist.", m_path.string());
			return false;
		}

		int channelCount;
		int width, height;

		stbi_set_flip_vertically_on_load(m_flip);

		byte* data = stbi_load(m_path.string().c_str(), &width, &height, &channelCount, 4);

		//if (stbi_failure_reason()) {
		//	Log::fatal("[~gTexture~x] STBI failure for image {}: {}", m_path.string(), stbi_failure_reason());
		//}

		//m_desc.setFormatFromChannelCount(channelCount);
		//if (channelCount != 4) {
		//	Log::fatal("[~gTexture~x] Unsupported image bit-depth {} {}", channelCount, m_path.string());
		//	stbi_image_free(data);
		//	return false;
		//}

		if (channelCount < 1 || channelCount > 4) Log::error("[Texture] unsupported image channel count ({}) {}.", channelCount, m_path.string());

		uint32_t size = 4 * width * height;

		if (data) {
			m_channelCount = channelCount;
			m_width = width;
			m_height = height;
			m_buffer = Buffer<byte>::copy(data, size);
			stbi_image_free(data);
			Log::info("[Texture] loaded {}", m_path.string());
		} else Log::error("[Texture] failed to load {}", m_path.string());
		return true;
	}

	Ref<Asset> TextureLoader::onFinishLoad() {
		return Ref<Texture>::create(m_desc, m_width, m_height, m_buffer.data(), m_buffer.size(), TextureDataType::RAW);
	}

	Ref<Asset> TextureLoader::syncLoadAndInvalidate() {
		Ref<Texture> texture = onFinishLoad();
		Renderer::submit([loader = Ref<TextureLoader>(this), instance = Ref<Texture>(texture)] {instance->invalidate(); });
		return texture;
	}
}