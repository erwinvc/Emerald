#include "stdafx.h"

void Shader::OnImGUI() {
	Bind();
	if (ImGui::Button(Format_t("Reload##%s", m_name.c_str()))) {
		GetApp()->QueueTask([this] {Reload(); });
	}
	m_uniformBuffer.OnImGUI();
}