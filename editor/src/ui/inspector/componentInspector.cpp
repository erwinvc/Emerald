#include "eepch.h"
#include "componentInspector.h"
#include "ui/imguiManager.h"
#include "inspectorRegistry.h"
#include "ecs/components/component.h"
#include "ecs/components/transformComponent.h"

namespace emerald {
	void DefaultComponentInspector::draw(const std::vector<Component*>& components) {
		ImGui::Text(components[0]->getClassType().name());
	}
}