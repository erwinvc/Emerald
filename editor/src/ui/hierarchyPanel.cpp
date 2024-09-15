#include "eepch.h"
#include "hierarchyPanel.h"
#include "ui/imguiManager.h"
#include "ecs/entity.h"
#include "scene/sceneManager.h"
#include "ui/iconsFontAwesome.h"
#include "undoRedo.h"

namespace emerald {
	void HierarchyPanel::draw() {
		if (ImGui::Begin("Hierarchy", nullptr)) {

			static char searchString[128] = { 0 };
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x - 54);
			ImGui::InputTextWithHint(ICON_FA_FILTER, ICON_FA_SEARCH " Search...", searchString, 256, ImGuiInputTextFlags_EscapeClearsAll);
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_TIMES)) {
				memset(searchString, 0, sizeof(searchString));
			}
			if (ImGui::EmeraldButton("Add Entity", ImVec2(-FLT_MIN, 0))) {
				auto action = UndoRedo::createAction<Entity>("Add Entity");
				UUID entityID = SceneManager::getActiveScene()->getECS().getNewEntityID();
				action->addDoAction([entityID](Entity& entity) {entity = SceneManager::getActiveScene()->getECS().createEntityFromID(entityID, "Entity"); });
				action->addUndoAction([](Entity& entity) {SceneManager::getActiveScene()->getECS().destroyEntity(entity); });
				UndoRedo::commitAction(action);
			}
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::BorderSeparator(4);
			if (SceneManager::isSceneOpen()) m_hierarchyTree.render(SceneManager::getActiveScene(), searchString);
			ImGui::PopStyleVar();
		}
		ImGui::End();
	}

	void HierarchyPanel::update(Timestep ts) {
		if (SceneManager::isSceneOpen()) m_hierarchyTree.handleDelete();
	}

	std::vector<Entity>& HierarchyPanel::getSelectedEntities() {
		return m_hierarchyTree.getSelectedEntities();
	}
}