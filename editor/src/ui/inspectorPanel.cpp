#include "eepch.h"
#include "inspectorPanel.h"
#include "ui/imguiManager.h"
#include "hierarchyPanel.h"
#include <unordered_set>
#include "inspectorRegistry.h"
#include "utils/math/color.h"
#include "inspector/propertyDrawer.h"
#include "engine/ecs/components/metadataComponent.h"
#include "engine/ecs/components/component.h"
#include "engine/scene/sceneManager.h"

namespace emerald {
	static constexpr float MIN_FIRST_COLUMN_WIDTH = 50.0f;

	void renderComponentInspector(RTTIType componentType, const std::vector<Component*>& components) {
		InspectorRegistry::getInspector(componentType)(components);
	}

	void InspectorPanel::drawSplitter(uint32_t totalWidth) {
		ImGui::SetCursorPos(ImVec2(m_firstColumnWidth - 8, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImGui::InvisibleButton("VerticalSplitter", ImVec2(8, ImGui::GetContentRegionAvail().y));

		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

		if (ImGui::IsItemActive()) {
			float delta = ImGui::GetIO().MouseDelta.x;
			m_firstColumnWidth += delta;

			if (m_firstColumnWidth < MIN_FIRST_COLUMN_WIDTH)
				m_firstColumnWidth = MIN_FIRST_COLUMN_WIDTH;
			if (m_firstColumnWidth > totalWidth - MIN_FIRST_COLUMN_WIDTH - m_splitterWidth)
				m_firstColumnWidth = totalWidth - MIN_FIRST_COLUMN_WIDTH - m_splitterWidth;
		}

		ImGui::PopStyleVar();
	}

	void InspectorPanel::drawInspectorHeader(std::vector<Entity>& selectedEntities) {
		Ref<Scene>& scene = SceneManager::getActiveScene();

		bool changed = false;
		static const char* xyzSymbols[3] = { "X", "Y", "Z" };

		std::vector<MetadataComponent*> nameComponents = scene->getECS().getComponentsInEntities<MetadataComponent>((std::vector<UUID>&)selectedEntities);

		ImGui::Spacing();
		PropertyDrawer::drawLabel("Name", nameComponents, &MetadataComponent::m_name, FixedString<64>("~"), DividerType::SINGLELINE);
	}

	void InspectorPanel::draw(HierarchyPanel* hierarchyPanel) {
		Ref<Scene>& scene = SceneManager::getActiveScene();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImVec2 padding = ImGui::GetStyle().WindowPadding;
		ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoNav)) {
			std::unordered_map<RTTIType, const ComponentTypeInfo*> componentInfo;
			std::vector<Entity>& selectedEntities = hierarchyPanel->getSelectedEntities();
			std::unordered_set<RTTIType> commonComponents;

			bool firstEntity = true;
			for (UUID entity : selectedEntities) {
				std::unordered_set<RTTIType> entityComponents;
				for (auto& [type, componentArrayBase] : scene->getECS().getComponentArrays()) {
					if (Component* comp = componentArrayBase->getComponentAsBase(entity)) {
						if (comp->getComponentTypeInfo().category == ComponentCategory::INTERNAL) continue;
						componentInfo[type] = &comp->getComponentTypeInfo();
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

			float totalWidth = ImGui::GetContentRegionAvail().x;

			ImGui::Columns(2, "inspectorPanelColumns", false);
			ImGui::SetColumnWidth(0, m_firstColumnWidth);
			ImGui::SetCursorPosX(0);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0xff171717);
			ImGui::BeginChild("inspectorPanelColumn1", ImVec2(0, 0), ImGuiChildFlags_None);
			auto& selectedComponentTypes = m_inspectorTree.draw(componentInfo);

			drawSplitter((uint32_t)totalWidth);

			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::NextColumn();
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::LEFT, IM_COL32(0, 0, 0, 155), 6.0f, (uint32_t)m_firstColumnWidth);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
			ImGui::BeginChild("inspectorPanelColumn2", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoBackground);

			struct SelectedComponent {
				RTTIType type;
				std::vector<Component*> components;
			};

			std::vector<SelectedComponent> selectedComponents;
			if (!selectedComponentTypes.empty()) {
				drawInspectorHeader(selectedEntities);

				for (auto& selectedComponentType : selectedComponentTypes) {
					std::vector<Component*> components;
					components.reserve(selectedEntities.size());
					for (UUID entity : selectedEntities) {
						Component* component = scene->getECS().getComponentArray(selectedComponentType).getComponentAsBase(entity);
						if(component) components.push_back(component);
					}

					selectedComponents.push_back({ selectedComponentType, components });
				}
			}

			//Sort selected components by category and name based deterministic priority index
			std::sort(selectedComponents.begin(), selectedComponents.end(),
				[](const SelectedComponent& a, const SelectedComponent& b) {
				auto aa = a.components[0];
				auto bb = b.components[0];
				const ComponentTypeInfo& infoA = aa->getComponentTypeInfo();
				const ComponentTypeInfo& infoB = bb->getComponentTypeInfo();

				if (infoA.category != infoB.category) {
					return infoA.category < infoB.category;
				}

				return infoA.inspectorPriorityIndex < infoB.inspectorPriorityIndex;
			});

			for (auto& selectedComponent : selectedComponents) {
				bool& collapsed = selectedComponent.components[0]->getComponentTypeInfo().collapsedInInspector;
				ImGui::SetNextItemOpen(!collapsed);
				collapsed = !ImGui::CollapsingHeader(selectedComponent.components[0]->getComponentTypeInfo().name.c_str());
				if (!collapsed) {
					ImGui::Indent(10.0f);
					renderComponentInspector(selectedComponent.type, selectedComponent.components);
					ImGui::Unindent(10.0f);
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar(2);
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}