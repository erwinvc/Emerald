#pragma once

class ShaderLoader : public AssetLoader {
private:
	String m_file;
	bool m_hasGeometry;
	bool m_hasTessellation;

public:
	ShaderLoader(const String& name, const String& file, bool hasGeometry = false, bool hasTessellation = false) : AssetLoader(name, true), m_file(file), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation) {}

	void AsyncLoad() override {

	}

	void SyncLoad(map<String, AssetBase*>& assets) override {
		GetShaderManager()->Create(m_name, m_file, m_hasGeometry, m_hasTessellation);
	}

	float GetProgress() override {
		return 0;
	}
};