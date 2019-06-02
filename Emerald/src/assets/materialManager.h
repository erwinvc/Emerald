#pragma once

class MaterialManager {
private:
    Material* m_nullMaterial;
public:
	~MaterialManager() { DELETE(m_nullMaterial); }
    void Initialize() {
        m_nullMaterial = NEW(Material());
    }

    Material* GetNullMaterial() { return m_nullMaterial; }

};

MaterialManager* GetMaterialManager();