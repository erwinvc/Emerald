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
	void Initialize() {
		if (m_initialized) return;
		//m_nullMaterial = NEW(BasicMaterial());

		LOG("[~GMaterial~x] Material Manager initialized");
		m_initialized = true;
	}

	//template<typename T>
	//AssetRef<T> Create(const String& name) {
	//	m_materials[name] = NEW(T());
	//	return (T*)m_materials[name];
	//}

	Material* GetBasicMaterial(Shader* shader) {
		if (m_defaultMaterials.find(shader->GetName()) == m_defaultMaterials.end()) {
			m_defaultMaterials[shader->GetName()] = NEW(Material(Format("%s_Default", shader->GetName().c_str()), shader, m_globalID++));
		}
		return m_defaultMaterials[shader->GetName()];
	}

	Material* Create(const String& name, Shader* shader);
	Material* Create(const String& name);

	AssetRef<Material> Get(const String& name) {
		return m_materials[name];
	}
};

static MaterialManager* GetMaterialManager() { return MaterialManager::GetInstance(); }