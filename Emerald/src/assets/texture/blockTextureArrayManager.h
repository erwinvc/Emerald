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
	~BlockTextureArrayManager();
	
public:
	void Initialize();
	void AddTexture(const String& name, const LoadedTextureArrayTexture& albedo, const LoadedTextureArrayTexture& normal, const LoadedTextureArrayTexture& roughness, const LoadedTextureArrayTexture& metallic, const LoadedTextureArrayTexture& emission);
	int Get(const String& name);
	void GenerateMipmaps();
	void Bind();
	
	inline TextureArray* GetAlbedo() { return m_albedoArray; }
	inline TextureArray* GetNormal() { return m_normalArray; }
	inline TextureArray* GetRoughness() { return m_roughnessArray; }
	inline TextureArray* GetMetallic() { return m_metallicArray; }
	inline TextureArray* GetEmission() { return m_emissionArray; }
};

static BlockTextureArrayManager* GetTileMaterialManager() { return BlockTextureArrayManager::GetInstance(); }