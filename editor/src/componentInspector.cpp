#include "eepch.h"
#include "componentInspector.h"
#include "ui/imguiManager.h"
#include "glm/gtc/type_ptr.inl"
#include <entities/components.h>
#include "inspectorRegistry.h"

namespace emerald {
	void DefaultComponentInspector::draw(const std::vector<Component*>& components) {
		ImGui::Text(components[0]->getClassType().name());
	}
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
	bool drawMulti(std::string_view name, const std::vector<Q*>& elements, PropertyType T::* member, PropertyType defaultValue) {
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

	class TransformComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components) {
			if (components.empty()) return;

			bool changed = false;
			changed |= drawMulti("Position", components, &TransformComponent::m_position, glm::vec3(0, 0, 0));
			changed |= drawMulti("Rotation", components, &TransformComponent::m_rotation, glm::vec3(0, 0, 0));
			changed |= drawMulti("Scale", components, &TransformComponent::m_scale, glm::vec3(0, 0, 0));

			if (changed) {
				for (Component* component : components) {
					((TransformComponent*)component)->markDirty();
				}
			}
		}
	};

	REGISTER_COMPONENT_INSPECTOR(TransformComponent, TransformComponentInspector);
}