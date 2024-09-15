#include "eepch.h"
#include "inspectorPanel.h"
#include "ui/imguiManager.h"
#include "hierarchyPanel.h"

namespace emerald {
	void InspectorPanel::draw(HierarchyPanel* hierarchyPanel) {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);


			//std::vector<UUID> selectedEntities = hierarchyPanel->getSelectedEntities();
			//std::unordered_set<RTTIType> commonComponents;
			//
			//bool firstEntity = true;
			//for (UUID entity : selectedEntities) {
			//	std::unordered_set<RTTIType> entityComponents;
			//	for (auto& pair : m_componentArrays) {
			//		if (pair.second->hasComponent(entity)) {
			//			entityComponents.insert(pair.first);
			//		}
			//	}
			//
			//	if (firstEntity) {
			//		commonComponents = entityComponents;
			//		firstEntity = false;
			//	} else {
			//		std::unordered_set<RTTIType> intersection;
			//		for (auto& type : commonComponents) {
			//			if (entityComponents.count(type)) {
			//				intersection.insert(type);
			//			}
			//		}
			//		commonComponents = intersection;
			//	}
			//}
			//
			//
			//if (hierarchyPanel->getSelectedEntity()) {
			//	auto& entity = hierarchyPanel->getSelectedEntity();
			//	ImGui::Text("Entity ID: %d", entity->getID());
			//	ImGui::Text("Name: %s", entity->getName().c_str());
			//	ImGui::BorderSeparator(4);
			//	ImGui::Spacing();
			//
			//	if (ImGui::CollapsingHeader("Transform")) {
			//		auto& transform = entity->getComponent<TransformComponent>();
			//		ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
			//		ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.1f);
			//		ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.1f);
			//	}
			//
			//	if (ImGui::CollapsingHeader("Sprite Renderer")) {
			//		auto& spriteRenderer = entity->getComponent<SpriteRendererComponent>();
			//		ImGui::ColorEdit4("Color", glm::value_ptr(spriteRenderer.color));
			//		ImGui::DragFloat("Tiling Factor", &spriteRenderer.tilingFactor, 0.1f);
			//	}
			//
			//	if (ImGui::CollapsingHeader("Camera")) {
			//		auto& camera = entity->getComponent<CameraComponent>();
			//		ImGui::Checkbox("Primary", &camera.primary);
			//		ImGui::Checkbox("Fixed Aspect Ratio", &camera.fixedAspectRatio);
			//		ImGui::DragFloat("Aspect Ratio", &camera.aspectRatio, 0.1f);
			//		ImGui::DragFloat("Orthographic Size", &camera.orthographicSize, 0.1f);
			//		ImGui::DragFloat("Near Clip", &camera.nearClip, 0.1f);
			//		ImGui::DragFloat("Far Clip", &camera.farClip, 0.1f);
			//	}
			//
			//	if (ImGui::CollapsingHeader("Script")) {
			//		auto& script = entity->getComponent<ScriptComponent>();
			//		ImGui::Text("Script Name: %s", script.moduleName.c_str());
			//	}
			//}
		}
		ImGui::End();
	}
}