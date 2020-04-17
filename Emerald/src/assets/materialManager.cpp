#include "stdafx.h"

Material* MaterialManager::Create(const String& name, Shader* shader) {
	m_materials[name] = NEW(Material(name, shader));
	return m_materials[name];
}

Material* MaterialManager::Create(const String& name) {
	m_materials[name] = NEW(Material(name, GetShaderManager()->Get("Geometry")));
	return m_materials[name];
}