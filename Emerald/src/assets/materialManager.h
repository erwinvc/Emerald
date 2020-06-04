#pragma once

class MaterialManager : public Singleton<MaterialManager> {
private:
	uint m_globalID = 0;
	map<String, Material*> m_materials;
	map<String, Material*> m_defaultMaterials;
	bool m_initialized = false;

	MaterialManager() {}
	~MaterialManager() {}
	friend Singleton;

public:
	void Initialize();
	Material* GetBasicMaterial(Shader* shader);

	Material* Create(const String& name, Shader* shader);
	Material* Create(const String& name);

	inline AssetRef<Material> Get(const String& name) { return m_materials[name]; }
};

static MaterialManager* GetMaterialManager() { return MaterialManager::GetInstance(); }