#include "eepch.h"
#include "transformComponentInspector.h"
#include "ecs/components/transformComponent.h"
#include "componentInspector.h"
#include "inspectorRegistry.h"

namespace emerald {
	class TransformComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components) {
			if (components.empty()) return;

			bool changed = false;
			changed |= drawMultiPropertyFloat3("Position", components, &TransformComponent::m_position, glm::vec3(0, 0, 0));
			changed |= drawMultiPropertyFloat3("Rotation", components, &TransformComponent::m_rotation, glm::vec3(0, 0, 0));
			changed |= drawMultiPropertyFloat3("Scale", components, &TransformComponent::m_scale, glm::vec3(0, 0, 0));

			if (changed) {
				for (Component* component : components) {
					((TransformComponent*)component)->markDirty();
				}
			}
		}
	};

	REGISTER_COMPONENT_INSPECTOR(TransformComponent, TransformComponentInspector);
}