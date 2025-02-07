#include "eepch.h"
#include "textureLoader.h"
#include "utils/system/fileSystem.h"
#include <stb_image.h>
#include "graphics/textures/texture.h"
#include "graphics/core/renderer.h"

namespace emerald {
	static constexpr uint32_t DESIRED_CHANNELS = 4;

	bool TextureLoader::loadDataFromFile(std::vector<byte>& rawData) {
		if (!FileSystem::doesFileExist(m_path)) {
			Log::error("[TextureLoader] File does not exist: {}", m_path.u8string());
			return false;
		}

		std::ifstream file(m_path.wstring(), std::ios::binary);
		if (!file.is_open()) {
			Log::error("[TextureLoader] Cannot open file: {}", m_path.u8string());
			return false;
		}

		file.seekg(0, std::ios::end);
		const std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (fileSize <= 0) {
			Log::error("[TextureLoader] File is empty or invalid size: {}", m_path.u8string());
			return false;
		}

		rawData.resize((size_t)fileSize);
		if (!file.read(reinterpret_cast<char*>(rawData.data()), fileSize)) {
			Log::error("[TextureLoader] Failed to read file: {}", m_path.u8string());
			return false;
		}

		return true;
	}

	bool TextureLoader::decodeImageData(const std::vector<byte>& rawData) {
		int width = 0, height = 0, channelCount = 0;
		unsigned char* data = stbi_load_from_memory((const stbi_uc*)rawData.data(), (int)rawData.size(), &width, &height, &channelCount, DESIRED_CHANNELS);

		if (!data) {
			Log::error("[TextureLoader] STB failed to load image data: {}",
				m_path.u8string());
			return false;
		}

		if (channelCount < 1 || channelCount > 4) {
			Log::error("[TextureLoader] Unsupported channel count ({}) in {}",
				channelCount, m_path.u8string());
			stbi_image_free(data);
			return false;
		}

		const uint32_t size = DESIRED_CHANNELS * width * height; 
		m_channelCount = DESIRED_CHANNELS;
		m_width = static_cast<uint32_t>(width);
		m_height = static_cast<uint32_t>(height);
		m_buffer = Buffer<byte>::copy(data, size);

		stbi_image_free(data);
		return true;
	}

	Expected<Empty> TextureLoader::onBeginLoad() {
		stbi_set_flip_vertically_on_load(m_flip);

		std::vector<byte> rawData;
		if (!m_loadFromMemory) {
			if (!loadDataFromFile(rawData)) {
				return Unexpected("[TextureLoader] Failed to load file: {}", m_path.u8string());
			}
		} else {
			if (!m_data || m_dataSize == 0) {
				return Unexpected("[TextureLoader] Memory data is null or size is zero.");
			}
			rawData.assign(m_data, m_data + m_dataSize);
		}

		if (!decodeImageData(rawData)) {
			return Unexpected("[TextureLoader] Failed to decode image data: {}", m_path.u8string());
		}

		return Expected<Empty>();
	}

	Expected<Ref<Asset>> TextureLoader::onFinishLoad(){
		Ref<Texture> texture = Ref<Texture>::create(m_desc, m_width, m_height, m_buffer.data(), m_buffer.size());
		Renderer::submit([instance = Ref<Texture>(texture)] {instance->invalidate(); });
		return texture.as<Asset>();
	}
}