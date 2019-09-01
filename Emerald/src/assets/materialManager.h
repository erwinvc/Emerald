#pragma once

class MaterialManager : public Singleton<MaterialManager> {
private:
	map<String, Material*> m_materials;
	BasicMaterial* m_nullMaterial;

	MaterialManager() {}
	~MaterialManager() {}
	friend Singleton;

public:
	void Initialize() {
		m_nullMaterial = NEW(BasicMaterial());
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