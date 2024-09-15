#include "eepch.h"
#include "inspectorPanel.h"
#include "ui/imguiManager.h"
#include "hierarchyPanel.h"
#include <unordered_set>
#include "componentInspector.h"
#include "inspectorRegistry.h"

namespace emerald {
	void renderComponentInspector(RTTIType componentType, const std::vector<Component*>& components) {
		InspectorRegistry::getInspector(componentType)(components);
	}

	void InspectorPanel::draw(const Ref<Scene> scene, HierarchyPanel* hierarchyPanel) {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);

			std::vector<Entity>& selectedEntities = hierarchyPanel->getSelectedEntities();
			std::unordered_set<RTTIType> commonComponents;

			bool firstEntity = true;
			for (UUID entity : selectedEntities) {
				std::unordered_set<RTTIType> entityComponents;
				for (auto& [type, componentArray] : scene->getECS().getComponentArrays()) {
					if (componentArray->has(entity)) {
						entityComponents.insert(type);
					}
				}

				if (firstEntity) {
					commonComponents = entityComponents;
					firstEntity = false;
				} else {
					std::unordered_set<RTTIType> intersection;
					for (auto& type : commonComponents) {
						if (entityComponents.count(type)) {
							intersection.insert(type);
						}
					}
					commonComponents = intersection;
				}
			}

			for (RTTIType componentType : commonComponents) {
				// Get components of this type from all entities
				std::vector<Component*> components;
				for (UUID entity : selectedEntities) {
					Component* component = scene->getECS().getComponentArray(componentType).getRaw(entity);
					if (component) {
						components.push_back(component);
					}
				}

				// Render inspector for this component type
				renderComponentInspector(componentType, components);
			}
		}
		ImGui::End();
	}
}