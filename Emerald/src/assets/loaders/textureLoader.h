#pragma once

class AssetManager;
class TextureLoader : public AssetLoader {
private:
	TextureParameters m_params;
	String m_file;
	byte* m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channelCount = 0;
	bool m_mipmaps;
public:
	TextureLoader(const String& name, const String& file, bool mipmaps, const TextureParameters& params = TextureParameters()) : AssetLoader(name), m_file(file), m_params(params), m_mipmaps(mipmaps) {}
	~TextureLoader() {
		delete[] m_data;
	}
	void AsyncLoad() override;
	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};