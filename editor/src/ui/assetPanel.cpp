#include "eepch.h"
#include "assetPanel.h"
#include "ui/imguiManager.h"

namespace emerald {
	void AssetPanel::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		}
		ImGui::End();
	}
}