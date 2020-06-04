#pragma once

class AssetManager;
class TextureLoader : public AssetLoader {
private:
	TextureParameters m_params;
	Path m_filePath;
	byte* m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channelCount = 0;
	bool m_mipmaps;
	bool m_keepData;
public:
	TextureLoader() : AssetLoader(""), m_filePath(""), m_params(TextureParameters()), m_mipmaps(false), m_keepData(false) {}
	TextureLoader(const String& name, const Path& filePath, bool mipmaps, const TextureParameters& params = TextureParameters(), bool keepData = false) : AssetLoader(name), m_filePath(filePath), m_params(params), m_mipmaps(mipmaps), m_keepData(keepData) {}
	~TextureLoader();
	
	void AsyncLoad() override;
	void SyncLoad(AssetManager* manager) override;
	float GetProgress() override;
};