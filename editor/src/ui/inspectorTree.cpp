#include "eepch.h"
#include "inspectorTree.h"
#include "ui/imguiManager.h"
#include "util/color.h"
#include "ui/iconsFontAwesome.h"
#include "imguiProfiler/IconsFontAwesome4.h"

namespace emerald {
	std::vector<RTTIType>& InspectorTree::draw(std::unordered_map<RTTIType, const ComponentTypeInfo*> components) {
		std::map<ComponentCategory, std::vector<std::pair<RTTIType, const ComponentTypeInfo*>>> componentsByCategory;

		for (const auto& [type, info] : components) {
			if (info->category == ComponentCategory::INTERNAL) {
				continue;
			}
			componentsByCategory[info->category].emplace_back(type, info);
		}

		m_idToRTTITypeMap.clear();

		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 10));
		//ImGui::ItemRowsBackground(-1, IM_COL32(28, 28, 128, 255));
		bool first = true;
		ImGui::Dummy(ImVec2(0, -8));
		for (const auto& [category, componentsInCategory] : componentsByCategory) {

			//Draw category header
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.07f, 0.07f, 0.07f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.07f, 0.07f, 0.07f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (!first) ImGui::Dummy(ImVec2(0, 8));
			first = false;
			ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
			ImGui::Selectable(getComponentCategoryName(category).data(), false, ImGuiSelectableFlags_SpanAvailWidth);
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImVec2 p_min = ImGui::GetItemRectMin();
			ImVec2 p_max = ImGui::GetItemRectMax();
			ImGui::PopStyleColor(3);

			//Handle selection
			bool selectAll = false;
			if (ImGui::IsItemClicked()) {
				if (ImGui::GetIO().KeyCtrl) {
					for (const auto& [type, info] : componentsInCategory) {
						selectAll = true;
					}
				} else {
					for (auto& [id, selected] : m_selectedIds) {
						m_selectedIds[id] = false;
					}
					for (const auto& [type, info] : componentsInCategory) {
						selectAll = true;
					}
				}
			}

			// Draw components in category
			for (const auto& [type, info] : componentsInCategory) {
				ImGuiTreeNodeFlags componentFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;

				ImGuiID componentId = ImGui::GetID((void*)(intptr_t)type.hash_code());

				m_idToRTTITypeMap.insert({ componentId, type });

				if (!m_selectedIds.contains(componentId)) m_selectedIds[componentId] = true;
				bool isSelected = m_selectedIds[componentId] || selectAll;

				if (isSelected) {
					componentFlags |= ImGuiTreeNodeFlags_Selected;
				}

				static bool b;
				bool newIsSelected = isSelected;
				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8);
				ImGui::Indent();
				ImGui::Selectable(info->name.data(), &newIsSelected);
				ImGui::Unindent();
				ImGui::PopStyleVar();

				if (isSelected != newIsSelected) {
					if (!newIsSelected) {
						if(ImGui::GetIO().KeyCtrl) m_selectedIds[componentId] = newIsSelected;
						else if (getSelectedComponentCount() > 1) {
							clearSelection();
							m_selectedIds[componentId] = true;
						}
					} else {
						if (!ImGui::GetIO().KeyCtrl) {
							clearSelection();
						}
						m_selectedIds[componentId] = newIsSelected;
					}
				}
				if (selectAll) {
					m_selectedIds[componentId] = true;
				}
			}
		}

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();

		m_selectedComponents.clear();
		for (auto& [id, selected] : m_selectedIds) {
			if (!selected)continue;
			auto it = m_idToRTTITypeMap.find(id);
			if (it != m_idToRTTITypeMap.end()) {
				RTTIType type = it->second;
				m_selectedComponents.push_back(type);
			}
		}

		return m_selectedComponents;
	}

	void InspectorTree::clearSelection() {
		for (auto& [id, selected] : m_selectedIds) {
			m_selectedIds[id] = false;
		}
	}

	uint32_t InspectorTree::getSelectedComponentCount() const {
		uint32_t count = 0;
		for (auto& [id, selected] : m_selectedIds) {
			if (selected) {
				count++;
			}
		}
		return count;
	}

	std::string_view InspectorTree::getComponentCategoryName(ComponentCategory category) {
		switch (category) {
			case ComponentCategory::INFO: return ICON_FA_INFO " info";
			case ComponentCategory::GRAPHICS: return  ICON_FA_CHART_AREA " graphics";
			case ComponentCategory::PHYSICS: return P_ICON_FA_CODEPEN " physics";
			case ComponentCategory::SCRIPTING: return ICON_FA_CODE " scripting";
			case ComponentCategory::OTHER: return "Other";
			default: return "Unknown";
		}
	}
}