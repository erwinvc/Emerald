#pragma once

struct LoadedTextureArrayTexture {
	Path path;
	byte* data;
	TextureParameters params;
	glm::uvec2 size;
};

class BlockTextureArrayLoader : public AssetLoader {
	String m_file;
	LoadedTextureArrayTexture m_albedo;
	LoadedTextureArrayTexture m_normal;
	LoadedTextureArrayTexture m_roughness;
	LoadedTextureArrayTexture m_metallic;
	LoadedTextureArrayTexture m_emission;

public:
	BlockTextureArrayLoader(const String& name, const String& file) : AssetLoader(name), m_file(file) {}
	~BlockTextureArrayLoader() {
		delete[] m_albedo.data;
		delete[] m_normal.data;
		delete[] m_roughness.data;
		delete[] m_metallic.data;
		delete[] m_emission.data;
	}

	
	void LoadAsyncTexture(LoadedTextureArrayTexture& loadedTextureData, const String& type) {
		loadedTextureData.path = Format("res/%s_%s.png", m_file.c_str(), type.c_str());
		TextureUtils::LoadTexture(loadedTextureData.path, true, [&](const LoadedTexture& data) {
			loadedTextureData.params.SetDataFormatFromChannelCount(data.m_channelCount);
			int size = data.m_height * data.m_width * data.m_channelCount;
			loadedTextureData.size = glm::uvec2(data.m_width, data.m_height);
			loadedTextureData.data = new byte[size];
			memcpy(loadedTextureData.data, data.m_data, size);
		});
	}
	
	void AsyncLoad() {
		LoadAsyncTexture(m_albedo, "albedo");
		LoadAsyncTexture(m_normal, "normal");
		LoadAsyncTexture(m_metallic, "metallic");
		LoadAsyncTexture(m_roughness, "roughness");
		LoadAsyncTexture(m_emission, "emission");
	}

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};