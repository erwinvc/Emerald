#pragma once

class PBRTextureLoader : public AssetLoader {
	String m_file;
	String m_extention;
	byte* m_albedo = nullptr;
	byte* m_normal = nullptr;
	byte* m_roughness = nullptr;
	byte* m_metallic = nullptr;
	byte* m_emission = nullptr;
	glm::vec2 m_sizes[5];
	int m_channelCounts[5];

	void AddAssetToManager(AssetManager* manager, const String& name, byte* data, int index, TextureParameters& params);

public:
	PBRTextureLoader(const String& name, const String& file, const String& extention) : AssetLoader(name), m_file(file), m_extention(extention) {}
	~PBRTextureLoader() {
		delete[] m_albedo;
		delete[] m_normal;
		delete[] m_roughness;
		delete[] m_metallic;
		delete[] m_emission;
	}

	void AsyncLoad() {
		TextureUtils::LoadTexture(Format("res/%s_albedo%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] PBR texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_albedo.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * data.m_channelCount;
			m_sizes[0] = glm::vec2((float)data.m_width, (float)data.m_height);
			m_channelCounts[0] = data.m_channelCount;
			m_albedo = new byte[size];
			memcpy(m_albedo, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_normal%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] PBR texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_normal.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * data.m_channelCount;
			m_sizes[1] = glm::vec2((float)data.m_width, (float)data.m_height);
			m_channelCounts[1] = data.m_channelCount;
			m_normal = new byte[size];
			memcpy(m_normal, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_roughness%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] PBR texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_roughness.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * data.m_channelCount;
			m_sizes[2] = glm::vec2((float)data.m_width, (float)data.m_height);
			m_channelCounts[2] = data.m_channelCount;
			m_roughness = new byte[size];
			memcpy(m_roughness, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_metallic%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] PBR texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_metallic.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * data.m_channelCount;
			m_sizes[3] = glm::vec2((float)data.m_width, (float)data.m_height);
			m_channelCounts[3] = data.m_channelCount;
			m_metallic = new byte[size];
			memcpy(m_metallic, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/%s_emission%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] PBR texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_emission.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * data.m_channelCount;
			m_sizes[4] = glm::vec2((float)data.m_width, (float)data.m_height);
			m_channelCounts[4] = data.m_channelCount;
			m_emission = new byte[size];
			memcpy(m_emission, data.m_data, size);
		});
	}

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};