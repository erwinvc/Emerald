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
	~BlockTextureArrayLoader();

	void LoadAsyncTexture(LoadedTextureArrayTexture& loadedTextureData, const String& type);
	void AsyncLoad();
	void SyncLoad(AssetManager* manager) override;
	float GetProgress() override;
};