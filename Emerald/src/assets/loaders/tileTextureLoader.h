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
		TextureUtils::LoadTexture(Format("tiles/%s_albedo.png", m_name.c_str()), 1, [this](byte* data, uint width, uint height) {
			int size = 4 * width * height;
			m_albedo = new byte[size];
			memcpy(m_albedo, data, size);
		});

		TextureUtils::LoadTexture(Format("tiles/%s_normal.png", m_name.c_str()), 1, [this](byte* data, uint width, uint height) {
			int size = 4 * width * height;
			m_normal = new byte[size];
			memcpy(m_normal, data, size);
		});

		TextureUtils::LoadTexture(Format("tiles/%s_specular.png", m_name.c_str()), 1, [this](byte* data, uint width, uint height) {
			int size = 4 * width * height;
			m_specular = new byte[size];
			memcpy(m_specular, data, size);
		});

		TextureUtils::LoadTexture(Format("tiles/%s_emission.png", m_name.c_str()), 1, [this](byte* data, uint width, uint height) {
			int size = 4 * width * height;
			m_emission = new byte[size];
			memcpy(m_emission, data, size);
		});
	}

	void SyncLoad(map<String, AssetBase*>& assets) {
		GetTileTextureManager()->AddTexture(m_name, m_albedo, m_normal, m_specular, m_emission);
	}

	float GetProgress() override {
		return 0;
	}
};