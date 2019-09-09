#pragma once

class ShaderLoader : public AssetLoader {
private:
	String m_file;
	bool m_a;
public:
	ShaderLoader(const String& name, const String& file, bool a = false) : AssetLoader(name, true), m_file(file), m_a(a) {}

	void AsyncLoad() override {

	}

	void SyncLoad(map<String, AssetBase*>& assets) override {
		if (m_a) {
			GetShaderManager()->Create(m_name, m_file, true);
		} else GetShaderManager()->Create(m_name, m_file, false);
	}

	float GetProgress() override {
		return 0;
	}
};