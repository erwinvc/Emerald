#pragma once
#include "ui/imguiManager.h"

namespace emerald {
	class Component;

	class ComponentInspector {
	};

	class DefaultComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components);
	};

	template<typename PropertyType, typename T, typename Q>
	static bool getCommonPropertyValue(const std::vector<Q*>& components, PropertyType T::* member, PropertyType& outValue, PropertyType defaultValue) {
		if (components.empty()) return false;
		PropertyType firstValue = ((T*)components[0])->*member;
		for (size_t i = 1; i < components.size(); ++i) {
			if (!(((T*)components[i])->*member == firstValue)) {
				outValue = defaultValue;
				return false;
			}
		}
		outValue = firstValue;
		return true;
	}

	template<typename PropertyType, typename T, typename Q>
	static bool drawMultiPropertyFloat3(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue) {
		bool toRet = false;
		PropertyType value;
		bool commonValue = getCommonPropertyValue(elements, member, value, glm::vec3(0, 0, 0));

		if (!commonValue) ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);

		if (ImGui::DragFloat3(name.data(), glm::value_ptr(value), 0.1f)) {
			for (Q* element : elements) {
				((T*)element)->*member = value;
			}
			toRet = true;
		}
		if (!commonValue) ImGui::PopItemFlag();
		return toRet;
	}
};