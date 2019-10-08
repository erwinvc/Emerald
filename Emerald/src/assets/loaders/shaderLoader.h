#pragma once

class ShaderLoader : public AssetLoader {
private:
	String m_file;
	bool m_hasGeometry;
	bool m_hasTessellation;

public:
	ShaderLoader(const String& name, const String& file, bool hasGeometry = false, bool hasTessellation = false) : AssetLoader(name), m_file(file), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation) {}

	void AsyncLoad() override {

	}

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};