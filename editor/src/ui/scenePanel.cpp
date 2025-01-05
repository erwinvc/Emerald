#include "eepch.h"
#include "scenePanel.h"
#include "ui/imguiManager.h"

namespace emerald {
	void ScenePanel::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		}
		ImGui::End();
	}
}