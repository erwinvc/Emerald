#include "eepch.h"
#include "componentInspector.h"
#include "components.h"
#include "ui/imguiManager.h"
#include "glm/gtc/type_ptr.inl"

namespace emerald {
	class TransformComponentInspector : public ComponentInspector {
	public:
		void renderInspector(Component* component) override {
			TransformComponent* transform = static_cast<TransformComponent*>(component);

			ImGui::Text("Transform Component");
			ImGui::DragFloat3("Position", glm::value_ptr(transform->m_position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transform->m_rotation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transform->m_scale), 0.1f);
		}
	};

	COMPONENT_INSPECTOR(TransformComponent, TransformComponentInspector)
}