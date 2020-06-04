#include "stdafx.h"

Shader* ShaderManager::Create(const String& name, const String& file, bool hasGeometry, bool hasTessellation) {
	if (m_shaders[name]) {
		LOG("[~bShaders~x] ~1%s ~rshader has already been created", name.c_str());
		return m_shaders[name];
	}
	AssetRef<Shader> shader = NEW(Shader(name, file, hasGeometry, hasTessellation));
	m_shaders[name] = shader;
	m_shadersVector.push_back(shader);
	return shader;
}

Shader* ShaderManager::Get(const String& name) {
	Shader* toRet = m_shaders[name];
	return toRet;
}

void ShaderManager::ReloadShaderByFileName(const String& file) {
	for (int i = 0; i < m_shadersVector.size(); i++) {
		LOG("[~bShaders~x] Shader reload requested: %s : %s", file.c_str(), m_shadersVector[i]->m_filePath.GetFullPath().c_str());
		if (m_shadersVector[i]->m_filePath.GetFullPath().compare(file) == 0) {
			m_shadersVector[i]->Reload();
		}
	}
}
void ShaderManager::OnImGUI() {
	if (ImGui::BeginTabItem("Shaders")) {
		static int currentlySelectedShader = 0;
		if (ImGui::BeginCombo("Shader", m_shadersVector[currentlySelectedShader]->m_name.c_str())) {
			for (int n = 0; n < m_shadersVector.size(); n++) {
				bool is_selected = (currentlySelectedShader == n);
				if (ImGui::Selectable(m_shadersVector[n]->m_name.c_str(), is_selected))
					currentlySelectedShader = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Separator();
		m_shadersVector[currentlySelectedShader]->OnImGUI();
		ImGui::EndTabItem();
	}
}

