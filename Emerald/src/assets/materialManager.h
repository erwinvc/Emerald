#pragma once

class MaterialManager : public Singleton<MaterialManager> {
private:
	map<String, Material*> m_materials;
	BasicMaterial* m_nullMaterial;
	bool m_initialized = false;

	MaterialManager() {}
	~MaterialManager() {}
	friend Singleton;

public:
	void Initialize() {
		if (m_initialized) return;
		m_nullMaterial = NEW(BasicMaterial());
		LOG("[~GMaterial~x] Material Manager initialized");
		m_initialized = true;
	}

	AssetRef<BasicMaterial> GetNullMaterial() { return m_nullMaterial; }

	template<typename T>
	AssetRef<T> Create(const String& name) {
		m_materials[name] = NEW(T());
		return (T*)m_materials[name];
	}

	AssetRef<Material> Get(const String& name) {
		return m_materials[name];
	}
};

static MaterialManager* GetMaterialManager() { return MaterialManager::GetInstance(); }