#pragma once

class MaterialManager : public Singleton<MaterialManager> {
private:
	map<String, Material*> m_materials;
	Material* m_nullMaterial;

	MaterialManager() {}
	~MaterialManager() {}
	friend Singleton;

public:
	void Initialize() {
		m_nullMaterial = NEW(Material());
	}

	AssetRef<Material> GetNullMaterial() { return m_nullMaterial; }

	AssetRef<Material> Create(const String& name) {
		m_materials[name] = NEW(Material());
		return m_materials[name];
	}

	AssetRef<Material> Get(const String& name) {
		return m_materials[name];
	}
};

static MaterialManager* GetMaterialManager() { return MaterialManager::GetInstance(); }