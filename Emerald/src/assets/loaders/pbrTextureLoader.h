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
	~PBRTextureLoader();

	void AsyncLoad();
	void SyncLoad(AssetManager* manager) override;
	float GetProgress() override;
};