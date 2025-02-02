#include "eepch.h"
#include "componentInspector.h"
#include "inspectorRegistry.h"
#include "ui/imguiManager.h"
#include "engine/ecs/components/meshRendererComponent.h"
#include "propertyDrawer.h"

namespace emerald {
	class MeshRendererComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components) {
			if (components.empty()) return;

			ImGui::Text("Mesh:" "");
			ImGui::Button("Mesh");

			PropertyDrawer::drawInt("SubMesh index", components, &MeshRendererComponent::m_submeshIndex, 0u, DividerType::SINGLELINE);

		}
	};

	REGISTER_COMPONENT_INSPECTOR(MeshRendererComponent, MeshRendererComponentInspector);
}