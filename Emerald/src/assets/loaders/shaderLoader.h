#pragma once

class ShaderLoader : public AssetLoader {
private:
	Path m_file;
	bool m_hasGeometry;
	bool m_hasTessellation;

public:
	ShaderLoader(const String& name, const Path& file, bool hasGeometry = false, bool hasTessellation = false) : AssetLoader(name), m_file(file), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation) {}

	void AsyncLoad() override;
	void SyncLoad(AssetManager* manager) override;
	float GetProgress() override;
};