#include "eepch.h"
#include "ecs/components/meshRendererComponent.h"
#include "componentInspector.h"
#include "inspectorRegistry.h"

namespace emerald {
	class MeshRendererComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components) {
			if (components.empty()) return;

			ImGui::Text("Mesh:" "");
			ImGui::Button("Mesh");
		}
	};

	REGISTER_COMPONENT_INSPECTOR(MeshRendererComponent, MeshRendererComponentInspector);
}