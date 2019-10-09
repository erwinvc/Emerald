#pragma once

class TileMaterialManager : public Singleton<TileMaterialManager> {
private:
	static const uint TEXTURESIZE = 128;
	static const uint LAYERCOUNT = 128;
	TextureArray* m_albedoArray;
	TextureArray* m_normalArray;
	TextureArray* m_roughnessArray;
	TextureArray* m_metallicArray;
	TextureArray* m_emissionArray;

	map<String, int> m_textureIndices;

	friend Singleton;
	TileMaterialManager() {}
	~TileMaterialManager() {
		DELETE(m_albedoArray);
		DELETE(m_normalArray);
		DELETE(m_roughnessArray);
		DELETE(m_metallicArray);
		DELETE(m_emissionArray);
	}
public:

	void Initialize() {
		m_albedoArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(RGB, RGBA)));
		m_normalArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(RGB, RGBA)));
		m_roughnessArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(RGB, RGBA)));
		m_metallicArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(RGB, RGBA)));
		m_emissionArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(RGB, RGBA)));
	}

	void AddTexture(const String& name, byte* albedo, byte* normal, byte* roughness, byte* metallic, byte* emission) {
		int i1 = m_albedoArray->AddTexture(albedo);
		int i2 = m_normalArray->AddTexture(normal);
		int i3 = m_roughnessArray->AddTexture(roughness);
		int i4 = m_metallicArray->AddTexture(metallic);
		int i5 = m_emissionArray->AddTexture(emission);
		if ((i1 == i2) && (i1 == i3) && (i1 == i4) && (i1 == i5)) {
			m_textureIndices[name] = i1;
		} else LOG_ERROR("[~gTexture~x] Texture array registration index mismatch! %d %d %d %d %d", i1, i2, i3, i4, i5);
	}

	int Get(const String& name) {
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
		m_roughnessArray->GenerateMipmaps();
		m_metallicArray->GenerateMipmaps();
		m_emissionArray->GenerateMipmaps();
	}

	void Bind() {
		m_albedoArray->Bind(0);
		m_normalArray->Bind(1);
		m_roughnessArray->Bind(2);
		m_metallicArray->Bind(3);
		m_emissionArray->Bind(4);
	}
	const TextureArray* GetAlbedo() { return m_albedoArray; }
	const TextureArray* GetNormal() { return m_normalArray; }
	const TextureArray* GetRoughness() { return m_roughnessArray; }
	const TextureArray* GetMetallic() { return m_metallicArray; }
	const TextureArray* GetEmission() { return m_emissionArray; }
};

static TileMaterialManager* GetTileMaterialManager() { return TileMaterialManager::GetInstance(); }