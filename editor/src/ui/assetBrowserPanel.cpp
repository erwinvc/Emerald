#include "eepch.h"
#include "assetBrowserPanel.h"
#include "ui/imguiManager.h"

namespace emerald {
	void AssetBrowserPanel::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);

			AssetRegistry::getAsset()
		}
		ImGui::End();
	}
}