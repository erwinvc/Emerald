#pragma once
#include "scene/scene.h"

namespace emerald {
	class HierarchyPanel;

	class InspectorPanel {
	public:
		void draw(const Ref<Scene> scene, HierarchyPanel* hierarchyPanel);
	};
}