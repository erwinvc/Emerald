#pragma once

class ShaderManager : public Singleton<ShaderManager> {
private:
	map<String, AssetRef<Shader>> m_shaders;
	vector<AssetRef<Shader>> m_shadersVector;
	ShaderManager() {}
	~ShaderManager() {}
	friend Singleton;

public:
	Shader* Create(const String& name, const String& file, bool hasGeometry = false, bool hasTessellation = false);
	Shader* Get(const String& name);
	void ReloadShaderByFileName(const String& file);
	void OnImGUI();
};

inline ShaderManager* GetShaderManager() { return ShaderManager::GetInstance(); }