#include "stdafx.h"

void MaterialManager::Initialize() {
	if (m_initialized) return;
	LOG("[~GMaterial~x] Material Manager initialized");
	m_initialized = true;
}

Material* MaterialManager::GetBasicMaterial(Shader* shader) {
	if (!shader) LOG_ERROR("[~GMaterial~x] Shader is a nullptr");
	if (m_defaultMaterials.find(shader->GetName()) == m_defaultMaterials.end()) {
		m_defaultMaterials[shader->GetName()] = NEW(Material(Format("%s_Default", shader->GetName().c_str()), shader, m_globalID++));
	}
	return m_defaultMaterials[shader->GetName()];
}

Material* MaterialManager::Create(const String& name, Shader* shader) {
	m_materials[name] = NEW(Material(name, shader, m_globalID++));
	return m_materials[name];
}

Material* MaterialManager::Create(const String& name) {
	m_materials[name] = NEW(Material(name, GetShaderManager()->Get("Geometry"), m_globalID++));
	return m_materials[name];
}