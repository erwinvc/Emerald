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
			static const char* xyzSymbols[3] = { "X", "Y", "Z" };

			ImGui::Columns(2, "mycolumns3", false);  // 3-ways, no border
			ImGui::SetColumnWidth(0, 100);
			ImGui::BeginChild("test");
			ImGui::Button("Transform", ImVec2(100, 100));
			ImGui::Button("Name", ImVec2(100, 100));
			ImGui::Button("Test3", ImVec2(100, 100));
			ImGui::Button("Test4", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::Button("Test5", ImVec2(100, 100));
			ImGui::EndChild();
			ImGui::NextColumn();
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::LEFT);

			changed |= drawMultiPropertyFloat3("Position", components, &TransformComponent::m_position, glm::vec3(0, 0, 0), xyzSymbols);
			changed |= drawMultiPropertyFloat3("Rotation", components, &TransformComponent::m_rotation, glm::vec3(0, 0, 0), xyzSymbols);
			changed |= drawMultiPropertyFloat3("Scale", components, &TransformComponent::m_scale, glm::vec3(0, 0, 0), xyzSymbols);

			if (changed) {
				for (Component* component : components) {
					((TransformComponent*)component)->markDirty();
				}
			}

		}
	};

	REGISTER_COMPONENT_INSPECTOR(TransformComponent, TransformComponentInspector);
}