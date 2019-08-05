#pragma once

class MaterialManager : public Singleton<MaterialManager> {
private:
	map<String, Material*> m_materials;
	Material* m_nullMaterial;

	MaterialManager() {}
	~MaterialManager() { DELETE(m_nullMaterial); }
	friend Singleton;

public:
	void Initialize() {
		m_nullMaterial = NEW(Material());
	}

	Material* GetNullMaterial() { return m_nullMaterial; }

	Ref<Material> Create(const String& name) {
		Material* mat = NEW(Material());
		m_materials[name] = mat;
		return mat;
	}

	Ref<Material> Get(const String& name) {
		return m_materials[name];
	}
};

static MaterialManager* GetMaterialManager() { return MaterialManager::GetInstance(); }