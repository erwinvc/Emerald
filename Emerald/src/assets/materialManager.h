#pragma once
class MaterialManager {
private:
    Material* m_nullMaterial;
public:
    void Initialize() {
        m_nullMaterial = new Material();
    }

    Material* GetNullMaterial() { return m_nullMaterial; }
};

MaterialManager* GetMaterialManager();