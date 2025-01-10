#include "eepch.h"
#include "hierarchyPanel.h"
#include "ui/imguiManager.h"
#include "engine/ecs/core/entity.h"
#include "engine/scene/sceneManager.h"
#include "ui/iconsFontAwesome.h"
#include "utils/undoRedo.h"

namespace emerald {
	void HierarchyPanel::draw() {
		if (ImGui::Begin("Hierarchy", nullptr)) {

			static char searchString[128] = { 0 };

			if (ImGui::BeginTable("HierarchyColumns", 2, ImGuiTableFlags_None)) {
				ImGui::TableSetupColumn("InputText", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputTextWithHint(UNIQUE_IMGUI_LABEL(), ICON_FA_SEARCH " Search...", searchString, 256, ImGuiInputTextFlags_EscapeClearsAll);

				ImGui::TableSetColumnIndex(1);
				if (ImGui::Button(ICON_FA_TIMES)) {
					memset(searchString, 0, sizeof(searchString));
				}

				ImGui::EndTable();
			}

			if (ImGui::EmeraldButton("Add Entity", ImVec2(-FLT_MIN, 0))) {
				auto action = UndoRedo::createAction<Entity>("Add Entity");
				UUID entityID = SceneManager::getActiveScene()->getECS().getNewEntityID();
				action->addDoAction([entityID](Entity& entity) {entity = SceneManager::getActiveScene()->getECS().createEntity(entityID, "Entity"); });
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