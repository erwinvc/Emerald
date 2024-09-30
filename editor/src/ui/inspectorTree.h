#pragma once
#include <unordered_map>
#include <unordered_set>
#include "ecs/components/component.h"
#include "rtti.h"
#include "imgui.h"

namespace emerald {
	class InspectorTree {
	public:
		std::vector<RTTIType>& draw(std::unordered_map<RTTIType, const ComponentTypeInfo*> components);

	private:
		std::vector<RTTIType> m_selectedComponents;
		std::unordered_map<ImGuiID, RTTIType> m_idToRTTITypeMap;
		std::unordered_map<ImGuiID, bool> m_selectedIds;
		std::string_view getComponentCategoryName(ComponentCategory category);
		uint32_t getSelectedComponentCount() const;
		void clearSelection();
	};
}