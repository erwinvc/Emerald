#include "eepch.h"
#include "ecs/components/transformComponent.h"
#include "componentInspector.h"
#include "inspectorRegistry.h"
#include "propertyDrawer.h"

namespace emerald {
	class TransformComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components) {
			if (components.empty()) return;

			bool changed = false;
			static const char* xyzSymbols[3] = { "X", "Y", "Z" };

			changed |= PropertyDrawer::drawFloat3("Position", components, &TransformComponent::m_position, glm::vec3(0, 0, 0), xyzSymbols, DividerType::TOP);
			changed |= PropertyDrawer::drawFloat3("Rotation", components, &TransformComponent::m_rotation, glm::vec3(0, 0, 0), xyzSymbols, DividerType::MIDDLE);
			changed |= PropertyDrawer::drawFloat3("Scale", components, &TransformComponent::m_scale, glm::vec3(0, 0, 0), xyzSymbols, DividerType::BOTTOM);

			//ImGui::SeparatorText("Een fiets");
			//changed |= drawPropertyFloat3("Rotation", components, &TransformComponent::m_rotation, glm::vec3(0, 0, 0), xyzSymbols, DividerType::TOP);
			//changed |= drawPropertyFloat3("Scale", components, &TransformComponent::m_scale, glm::vec3(0, 0, 0), xyzSymbols, DividerType::BOTTOM);
			//
			//ImGui::SeparatorText("Een tweede fiets");
			//changed |= drawPropertyFloat3("Position", components, &TransformComponent::m_position, glm::vec3(0, 0, 0), xyzSymbols, DividerType::SINGLELINE);


			if (changed) {
				for (Component* component : components) {
					((TransformComponent*)component)->markDirty();
				}
			}
		}
	};

	REGISTER_COMPONENT_INSPECTOR(TransformComponent, TransformComponentInspector);
}