#include "eepch.h"
#include "dragDrop.h"
#include "ui/imguiManager.h"
#include "ui/iconsFontAwesome.h"

namespace emerald {
	static bool s_isAtvalidDropLocation = false;

	void DragDrop::handleDragDropVisuals() {
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsDragDropActive()) {
			if (!s_isAtvalidDropLocation) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
			}

			ImDrawList* drawList = ImGui::GetForegroundDrawList();

			ImVec2 mousePos = io.MousePos;
			const ImVec2 offset(10, 8);

			ImGuiManager::pushFont(AWESOME_S);
			ImU32 green = IM_COL32(100, 100, 100, 255);
			drawList->AddText(ImVec2(mousePos.x + offset.x, mousePos.y + offset.y), green, ICON_FA_BORDER_STYLE);
			ImGuiManager::popFont();
		}

		s_isAtvalidDropLocation = false;
	}

	void DragDrop::setIsAtValidDropLocation() {
		s_isAtvalidDropLocation = true;
	}
}