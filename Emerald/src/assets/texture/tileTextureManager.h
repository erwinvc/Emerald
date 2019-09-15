#pragma once

class TileTextureManager : public Singleton<TileTextureManager> {
private:
	static const uint TEXTURESIZE = 128;
	static const uint LAYERCOUNT = 128;
	TextureArray* m_albedoArray;
	TextureArray* m_normalArray;
	TextureArray* m_specularArray;
	TextureArray* m_emissionArray;

	map<String, int> m_textureIndices;

	friend Singleton;
	TileTextureManager() {}
	~TileTextureManager() {
		DELETE(m_albedoArray);
		DELETE(m_normalArray);
		DELETE(m_specularArray);
		DELETE(m_emissionArray);
	}
public:

	void Initialize() {
		m_albedoArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters()));
		m_normalArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters()));
		m_specularArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters()));
		m_emissionArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters()));
	}

	void AddTexture(const String& name, byte* albedo, byte* normal, byte* specular, byte* emission) {
		int i1 = m_albedoArray->AddTexture(albedo);
		int i2 = m_normalArray->AddTexture(normal);
		int i3 = m_specularArray->AddTexture(specular);
		int i4 = m_emissionArray->AddTexture(emission);
		if (i1 == (i2, i3, i4)) {
			m_textureIndices[name] = i1;
		} else LOG_ERROR("[~gTexture~x] Texture array registration index mismatch! %d %d %d %d", i1, i2, i3, i4);
	}

	int GetIndex(const String& name) {
		auto found = m_textureIndices.find(name);
		if (found != m_textureIndices.end()) {
			return m_textureIndices[name];
		}
		LOG_ERROR("[~gTexture~x] Tile texture index ~1%s~x not found", name.c_str());
		return -1;
	}

	void GenerateMipmaps() {
		m_albedoArray->GenerateMipmaps();
		m_normalArray->GenerateMipmaps();
		m_specularArray->GenerateMipmaps();
		m_emissionArray->GenerateMipmaps();
	}

	const TextureArray* GetAlbedo() { return m_albedoArray; }
	const TextureArray* GetNormal() { return m_normalArray; }
	const TextureArray* GetSpecular() { return m_specularArray; }
	const TextureArray* GetEmission() { return m_emissionArray; }
};

static TileTextureManager* GetTileTextureManager() { return TileTextureManager::GetInstance(); }