#pragma once

struct LoadedTextureArrayTexture;
class BlockTextureArrayManager : public Singleton<BlockTextureArrayManager> {
private:
	static const uint TEXTURESIZE = 64;
	static const uint LAYERCOUNT = 128;
	TextureArray* m_albedoArray;
	TextureArray* m_normalArray;
	TextureArray* m_roughnessArray;
	TextureArray* m_metallicArray;
	TextureArray* m_emissionArray;

	map<String, int> m_textureIndices;

	friend Singleton;
	BlockTextureArrayManager() {}
	~BlockTextureArrayManager() {
		DELETE(m_albedoArray);
		DELETE(m_normalArray);
		DELETE(m_roughnessArray);
		DELETE(m_metallicArray);
		DELETE(m_emissionArray);
	}
public:

	void Initialize() {
		m_albedoArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_SRGB, DATA_UNK, NEAREST)));
		m_normalArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RGB, DATA_UNK , LINEAR)));
		m_roughnessArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RED, DATA_UNK, NEAREST)));
		m_metallicArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RED, DATA_UNK, NEAREST)));
		m_emissionArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RGB, DATA_UNK, NEAREST)));
	}

	void AddTexture(const String& name, const LoadedTextureArrayTexture& albedo, const LoadedTextureArrayTexture& normal, const LoadedTextureArrayTexture& roughness, const LoadedTextureArrayTexture& metallic, const LoadedTextureArrayTexture& emission);

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
	TextureArray* GetAlbedo() { return m_albedoArray; }
	TextureArray* GetNormal() { return m_normalArray; }
	TextureArray* GetRoughness() { return m_roughnessArray; }
	TextureArray* GetMetallic() { return m_metallicArray; }
	TextureArray* GetEmission() { return m_emissionArray; }
};

static BlockTextureArrayManager* GetTileMaterialManager() { return BlockTextureArrayManager::GetInstance(); }