#pragma once

class PBRTextureLoader : public AssetLoader {
	String m_file;
	byte* m_albedo = nullptr;
	byte* m_normal = nullptr;
	byte* m_roughness = nullptr;
	byte* m_metallic = nullptr;
	byte* m_emission = nullptr;
	Vector2 m_sizes[5];

	void AddAssetToManager(AssetManager* manager, const String& name, byte* data, int index);

public:
	PBRTextureLoader(const String& name, const String& file) : AssetLoader(name), m_file(file) {}
	~PBRTextureLoader() {
		delete[] m_albedo;
		delete[] m_normal;
		delete[] m_roughness;
		delete[] m_metallic;
		delete[] m_emission;
	}

	void AsyncLoad() {
		TextureUtils::LoadTexture(Format("res/%s_albedo.png", m_file.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_albedo.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_sizes[0] = Vector2((float)data.m_width, (float)data.m_height);
			m_albedo = new byte[size];
			memcpy(m_albedo, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_normal.png", m_file.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_normal.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_sizes[1] = Vector2((float)data.m_width, (float)data.m_height);
			m_normal = new byte[size];
			memcpy(m_normal, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_roughness.png", m_file.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_roughness.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_sizes[2] = Vector2((float)data.m_width, (float)data.m_height);
			m_roughness = new byte[size];
			memcpy(m_roughness, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_metallic.png", m_file.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_metallic.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_sizes[3] = Vector2((float)data.m_width, (float)data.m_height);
			m_metallic = new byte[size];
			memcpy(m_metallic, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_emission.png", m_file.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_emission.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_sizes[4] = Vector2((float)data.m_width, (float)data.m_height);
			m_emission = new byte[size];
			memcpy(m_emission, data.m_data, size);
		});
	}

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};