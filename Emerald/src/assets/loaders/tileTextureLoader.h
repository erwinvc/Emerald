#pragma once

class TileTextureLoader : public AssetLoader {
	String m_name;
	byte* m_albedo;
	byte* m_normal;
	byte* m_specular;
	byte* m_emission;

public:
	TileTextureLoader(const String name, const String& file) : AssetLoader(name, true), m_name(file) {}
	~TileTextureLoader() {
		delete[] m_albedo;
		delete[] m_normal;
		delete[] m_specular;
		delete[] m_emission;
	}

	void AsyncLoad() {
		TextureUtils::LoadTexture(Format("res/tiles/%s_albedo.png", m_name.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_albedo.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_albedo = new byte[size];
			memcpy(m_albedo, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/tiles/%s_normal.png", m_name.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_normal.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_normal = new byte[size];
			memcpy(m_normal, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/tiles/%s_specular.png", m_name.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_specular.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_specular = new byte[size];
			memcpy(m_specular, data.m_data, size);
		});

		TextureUtils::LoadTexture(Format("res/tiles/%s_emission.png", m_name.c_str()), 1, [this](const LoadedTexture& data) {
			//if (data.m_channelCount != 4) LOG_ERROR("[~gTexture~x] tile texture needs 4 channels! %d found in ~1%s", data.m_channelCount, Format("%s_emission.png", m_name.c_str()).c_str());
			int size = data.m_height * data.m_width * 4;
			m_emission = new byte[size];
			memcpy(m_emission, data.m_data, size);
		});
	}

	void SyncLoad(map<String, AssetBase*>& assets) {
		GetTileTextureManager()->AddTexture(m_name, m_albedo, m_normal, m_specular, m_emission);
	}

	float GetProgress() override {
		return 0;
	}
};