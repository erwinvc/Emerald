#include "stdafx.h"

uint Shader::s_boundShader = 0xffffffff;

void Shader::OnImGUI() {
	Bind();
	if (ImGui::Button(Format_t("Reload##%s", m_name.c_str()))) {
		GetClient()->QueueTask([this] {Reload(); });
	}
	m_uniformBuffer.OnImGUI();
}