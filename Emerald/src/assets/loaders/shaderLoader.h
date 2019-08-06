#pragma once

class ShaderLoader : public AssetLoader {
private:
	String m_file;
public:
	ShaderLoader(const String& name, const String& file) : AssetLoader(name, true), m_file(file) {}

	void AsyncLoad() override {

	}

	void SyncLoad(map<String, AssetBase*>& assets) override {
		GetShaderManager()->Create(m_name, m_file);
	}

	float GetProgress() override {
		return 0;
	}
};