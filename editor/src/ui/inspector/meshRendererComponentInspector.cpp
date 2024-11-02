#include "eepch.h"
#include "componentInspector.h"
#include "inspectorRegistry.h"
#include "ui/imguiManager.h"
#include "engine/ecs/components/meshRendererComponent.h"

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